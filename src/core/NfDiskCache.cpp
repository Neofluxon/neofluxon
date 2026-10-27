/**
 * File name: NfDiskCache.cpp
 * Project: Neofluxon (a photography workflow software)
 *
 * Copyright (C) 2026 Iurie Nistor
 *
 * This file is part of Neofluxon.
 *
 * Neofluxon is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "NfDiskCache.h"
#include "NfImage.h"
#include "NfImageData.h"
#include "NfLimits.h"
#include "NfFileUtils.h"
#include "NfLogger.h"
#include "utils/NfScopedTimer.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>
#include <iterator>
#include <limits>
#include <optional>
#include <vector>

namespace NfCore {

namespace {

constexpr std::array<char, 8> magic{'N', 'F', 'D', 'C', 'A', 'C', 'H', 'E'};
constexpr std::uint32_t formatVersion = 1;
constexpr std::uint64_t headerSize = 8 + 6 * sizeof(std::uint32_t) + sizeof(std::uint64_t);

void writeU32(std::ostream& stream, std::uint32_t value)
{
        for (unsigned int shift = 0; shift < 32; shift += 8)
                stream.put(static_cast<char>((value >> shift) & 0xff));
}

void writeU64(std::ostream& stream, std::uint64_t value)
{
        for (unsigned int shift = 0; shift < 64; shift += 8)
                stream.put(static_cast<char>((value >> shift) & 0xff));
}

bool readU32(std::istream& stream, std::uint32_t& value)
{
        value = 0;
        for (unsigned int shift = 0; shift < 32; shift += 8) {
                const auto byte = stream.get();
                if (byte == std::char_traits<char>::eof())
                        return false;
                value |= static_cast<std::uint32_t>(static_cast<unsigned char>(byte)) << shift;
        }
        return true;
}

bool readU64(std::istream& stream, std::uint64_t& value)
{
        value = 0;
        for (unsigned int shift = 0; shift < 64; shift += 8) {
                const auto byte = stream.get();
                if (byte == std::char_traits<char>::eof())
                        return false;
                value |= static_cast<std::uint64_t>(
                                 static_cast<unsigned char>(byte)) << shift;
        }
        return true;
}

} // namespace

NfDiskCache::NfDiskCache(NfDiskCacheSettings settings)
        : m_settings{std::move(settings)}
        , m_worker{[this] { workerLoop(); }}
{
        NfScopedTimer timer{"NfDiskCache: disk cache initialization", m_settings.directory};

        std::vector<IndexedEntry> entries;
        indexEntries(CacheType::Thumbnail, entries);
        indexEntries(CacheType::Preview, entries);

        std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second > rhs.second;
        });

        for (const auto& [entry, modified] : entries) {
                m_lruOrder.push_back(entry);
                const auto it = std::prev(m_lruOrder.end());
                m_currentCacheSize += entry.size;
                m_lruOrderById.emplace(std::make_pair(entry.type, entry.id), it);
        }
}

NfDiskCache::~NfDiskCache()
{
        {
                std::lock_guard lock(m_queueMutex);
                m_stopping = true;
        }
        m_queueCondition.notify_one();
        if (m_worker.joinable())
                m_worker.join();
}

std::unique_ptr<NfImage> NfDiskCache::get(CacheType type, const NfPhotoId& id)
{
        std::lock_guard lock(m_mutex);
        const auto path = entryPath(type, id);
        const auto fileSize = NfFileUtils::fileSize(path);
        if (!fileSize) {
                removeEntry(type, id.value());
                return nullptr;
        }
        std::error_code error;

        std::ifstream file(path, std::ios::binary);
        if (!file)
                return nullptr;

        std::array<char, magic.size()> readMagic{};
        file.read(readMagic.data(), static_cast<std::streamsize>(readMagic.size()));
        std::uint32_t version{};
        std::uint32_t width{};
        std::uint32_t height{};
        std::uint32_t channels{};
        std::uint32_t orientation{};
        std::uint32_t imageFormat{};
        std::uint64_t dataSize{};
        if (!file || readMagic != magic
            || !readU32(file, version)
            || !readU32(file, width)
            || !readU32(file, height)
            || !readU32(file, channels)
            || !readU32(file, orientation)
            || !readU32(file, imageFormat)
            || !readU64(file, dataSize)
            || version != formatVersion
            || width == 0 || height == 0
            || width > NfLimits::maxImageDimension || height > NfLimits::maxImageDimension
            || channels == 0 || channels > 4
            || imageFormat > static_cast<std::uint32_t>(
                                     NfImageData::ImageFormat::Format_ARGB32_Premultiplied)
            || dataSize != static_cast<std::uint64_t>(width) * height * channels
            || *fileSize != headerSize + dataSize
            || dataSize > NfLimits::maxDecompressedImageSize) {
                file.close();
                if (std::filesystem::remove(path, error) && !error)
                        removeEntry(type, id.value());
                NF_LOG_ERROR("discarding invalid disk cache entry: " << path);
                return nullptr;
        }

        std::vector<unsigned char> pixels(static_cast<std::size_t>(dataSize));
        file.read(reinterpret_cast<char*>(pixels.data()),
                  static_cast<std::streamsize>(pixels.size()));
        if (!file) {
                file.close();
                if (std::filesystem::remove(path, error) && !error)
                        removeEntry(type, id.value());
                NF_LOG_ERROR("discarding truncated disk cache entry: " << path);
                return nullptr;
        }

        auto data = std::make_unique<NfImageData>(pixels.size());
        data->setData(pixels.data(), pixels.size());
        data->setWidth(static_cast<int>(width));
        data->setHeight(static_cast<int>(height));
        data->setChannels(static_cast<int>(channels));
        data->setOrientation(static_cast<int>(orientation));
        data->setFormat(static_cast<NfImageData::ImageFormat>(imageFormat));
        touchEntry(type, id.value());

        return std::make_unique<NfImage>(std::move(data));
}

void NfDiskCache::indexEntries(CacheType type, std::vector<IndexedEntry>& entries)
{
        const auto directory = m_settings.directory
                / (type == CacheType::Thumbnail ? "thumbnails" : "previews");

        std::error_code error;
        if (!std::filesystem::exists(directory, error)) {
                if (error) {
                        NF_LOG_ERROR("can't inspect disk cache directory: "
                                     << directory << ": " << error.message());
                }
                return;
        }

        for (std::filesystem::directory_iterator it(directory, error), end;
             !error && it != end; it.increment(error)) {
                if (!it->is_regular_file(error) || error || it->path().extension() != ".nfc")
                        continue;

                const auto fileSize = NfFileUtils::fileSize(it->path());
                if (!fileSize)
                        continue;
                const auto modified = it->last_write_time(error);
                if (error) {
                        error.clear();
                        continue;
                }

                // Get photo id value
                const auto name = it->path().stem().string();
                std::uint64_t id{};
                const auto [endName, parseError] = std::from_chars(name.data(),
                                                                   name.data() + name.size(),
                                                                   id);
                if (parseError != std::errc{} || endName != name.data() + name.size())
                        continue;

                entries.push_back({{type, id, *fileSize}, modified});
        }
}

void NfDiskCache::enqueue(CacheType type,
                          const NfPhotoId& id,
                          std::unique_ptr<NfImage> image)
{
        if (!image)
                return;

        {
                std::lock_guard cacheLock(m_mutex);
                std::lock_guard queueLock(m_queueMutex);
                if (m_stopping)
                        return;
                auto& queuedIds = type == CacheType::Thumbnail
                        ? m_queuedThumbnailIds : m_queuedPreviewIds;
                if (m_lruOrderById.contains(std::make_pair(type, id.value()))
                    || !queuedIds.insert(id.value()).second)
                        return;

                m_writeQueue.emplace_back(type, id, std::move(image));
        }

        m_queueCondition.notify_one();
}

void NfDiskCache::workerLoop()
{
        while (true) {
                std::optional<WriteRequest> request;
                {
                        std::unique_lock lock(m_queueMutex);
                        m_queueCondition.wait(lock, [this] {
                                return m_stopping || !m_writeQueue.empty();
                        });

                        if (m_writeQueue.empty()) {
                                if (m_stopping)
                                        return;
                                continue;
                        }

                        request.emplace(std::move(m_writeQueue.front()));
                        m_writeQueue.pop_front();
                }

                put(request->type, request->id, *request->image);
                removeQueuedId(request->id.value(), request->type);
        }
}

bool NfDiskCache::put(CacheType type, const NfPhotoId& id, const NfImage& image)
{
        if (!image.isValid())
                return false;

        const auto* data = image.getData();
        const auto fileSize = headerSize + data->size();
        if (fileSize > m_settings.maxSizeBytes) {
                NF_LOG_ERROR("file too big for disck cache. photo id = " << id.value());
                return false;
        }

        std::lock_guard lock(m_mutex);
        const auto destination = entryPath(type, id);
        std::error_code error;
        std::filesystem::create_directories(destination.parent_path(), error);
        if (error) {
                NF_LOG_ERROR("can't create disk cache directory: "
                             << destination.parent_path() << ": " << error.message());
                return false;
        }

        const auto space = std::filesystem::space(destination.parent_path(), error);
        if (error || fileSize > space.available) {
                NF_LOG_ERROR("disk cache write skipped, not enouth free space");
                return false;
        }

        const auto key = std::make_pair(type, id.value());
        const auto existingEntry = m_lruOrderById.find(key);
        const auto previousFileSize = existingEntry == m_lruOrderById.end()
                ? 0 : existingEntry->second->size;
        const auto currentSizeWithoutDestination =
                m_currentCacheSize - std::min(m_currentCacheSize, previousFileSize);
        const auto additionalBytes = fileSize > previousFileSize
                ? fileSize - previousFileSize : 0;

        const auto temporary = destination.string() + ".tmp";
        {
                std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
                if (!file) {
                        NF_LOG_ERROR("can't create disk cache entry: " << temporary);
                        return false;
                }
                file.write(magic.data(), static_cast<std::streamsize>(magic.size()));
                writeU32(file, formatVersion);
                writeU32(file, static_cast<std::uint32_t>(data->width()));
                writeU32(file, static_cast<std::uint32_t>(data->height()));
                writeU32(file, static_cast<std::uint32_t>(data->channels()));
                writeU32(file, static_cast<std::uint32_t>(data->orientation()));
                writeU32(file, static_cast<std::uint32_t>(data->format()));
                writeU64(file, data->size());
                file.write(reinterpret_cast<const char*>(data->data()),
                           static_cast<std::streamsize>(data->size()));
                file.flush();

                if (!file) {
                        file.close();
                        std::filesystem::remove(temporary, error);
                        NF_LOG_ERROR("can't write disk cache entry: " << temporary);
                        return false;
                }
        }

        const bool fits = currentSizeWithoutDestination <= m_settings.maxSizeBytes
                && fileSize <= m_settings.maxSizeBytes - currentSizeWithoutDestination;
        const bool isOk = fits || evictToFit(additionalBytes, type, id.value());

        if (!isOk) {
                std::filesystem::remove(temporary, error);
                NF_LOG_ERROR("disk cache quota cannot be met; skipping entry: "
                               << destination);
                return false;
        }

        std::filesystem::rename(temporary, destination, error);
        if (error) {
                std::filesystem::remove(destination, error);
                error.clear();
                std::filesystem::rename(temporary, destination, error);
        }

        if (error) {
                std::filesystem::remove(temporary, error);
                NF_LOG_ERROR("can't add disk cache entry: " << destination);
                return false;
        }

        if (existingEntry != m_lruOrderById.end()) {
                m_currentCacheSize -= existingEntry->second->size;
                existingEntry->second->size = fileSize;
                m_currentCacheSize += fileSize;
                m_lruOrder.splice(m_lruOrder.begin(), m_lruOrder,
                                      existingEntry->second);
        } else {
                m_lruOrder.push_front({type, id.value(), fileSize});
                m_lruOrderById.emplace(key, m_lruOrder.begin());
                m_currentCacheSize += fileSize;
        }
        return true;
}

bool NfDiskCache::evictToFit(std::uint64_t requiredBytes,
                             CacheType protectedType,
                             std::uint64_t protectedId)
{
        // Evict at least 10% of the disk cache
        size_t freeSpace = 0.1 * m_settings.maxSizeBytes;
        if (requiredBytes < freeSpace)
                requiredBytes = freeSpace;

        const auto protectedEntry =
                m_lruOrderById.find(std::make_pair(protectedType, protectedId));
        const auto protectedSize = protectedEntry == m_lruOrderById.end()
                ? 0 : protectedEntry->second->size;
        const auto targetSize = m_settings.maxSizeBytes - requiredBytes;

        while (!m_lruOrder.empty()
               && m_currentCacheSize - std::min(m_currentCacheSize, protectedSize)
                       > targetSize) {
                auto it = std::prev(m_lruOrder.end());
                if (it->type == protectedType && it->id == protectedId) {
                        if (it == m_lruOrder.begin())
                                break;
                        --it;
                }
                const auto type = it->type;
                const auto id = it->id;
                const auto path = entryPath(type, id);
                std::error_code error;
                if (std::filesystem::remove(path, error) && !error) {
                        NF_LOG_DEBUG("cache file evicted: " << path);
                        m_currentCacheSize -= it->size;
                        m_lruOrderById.erase(std::make_pair(type, id));
                        m_lruOrder.erase(it);
                } else {
                        m_lruOrder.splice(m_lruOrder.begin(), m_lruOrder, it);
                }
        }

        return m_currentCacheSize - std::min(m_currentCacheSize, protectedSize)
                <= targetSize;
}

void NfDiskCache::touchEntry(CacheType type, std::uint64_t id)
{
        const auto entry = m_lruOrderById.find(std::make_pair(type, id));
        if (entry == m_lruOrderById.end())
                return;
        m_lruOrder.splice(m_lruOrder.begin(), m_lruOrder, entry->second);
}

void NfDiskCache::removeEntry(CacheType type, std::uint64_t id)
{
        const auto entry = m_lruOrderById.find(std::make_pair(type, id));
        if (entry == m_lruOrderById.end())
                return;
        m_currentCacheSize -= std::min(m_currentCacheSize, entry->second->size);
        m_lruOrder.erase(entry->second);
        m_lruOrderById.erase(entry);
}

std::filesystem::path NfDiskCache::entryPath(CacheType type,
                                             const NfPhotoId& id) const
{
        return entryPath(type, id.value());
}

std::filesystem::path NfDiskCache::entryPath(CacheType type,
                                             std::uint64_t id) const
{
        const auto name = std::to_string(id) + ".nfc";
        const auto typeDirectory = type == CacheType::Thumbnail ? "thumbnails" : "previews";
        return m_settings.directory / typeDirectory / name;
}

void NfDiskCache::removeQueuedId(std::uint64_t id, CacheType type)
{
        std::lock_guard queueLock(m_queueMutex);
        auto& ids = type == CacheType::Thumbnail
                ? m_queuedThumbnailIds : m_queuedPreviewIds;
        ids.erase(id);
}

} // namespace NfCore
