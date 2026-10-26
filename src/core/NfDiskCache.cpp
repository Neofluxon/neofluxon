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
#include "NfLogger.h"

#include <algorithm>
#include <array>
#include <fstream>
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
                value |= static_cast<std::uint32_t>(
                                 static_cast<unsigned char>(byte)) << shift;
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

struct CacheFile {
        std::filesystem::path path;
        std::uint64_t size;
        std::filesystem::file_time_type modified;
};

} // namespace

NfDiskCache::NfDiskCache(NfDiskCacheSettings settings)
        : m_settings{std::move(settings)}
        , m_worker{[this] { workerLoop(); }}
{
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

void NfDiskCache::enqueue(CacheType type,
                          const NfPhotoId& id,
                          std::unique_ptr<NfImage> image)
{
        if (!image)
                return;

        {
                std::lock_guard lock(m_queueMutex);
                if (m_stopping)
                        return;
                const auto queued = std::find_if(
                        m_writeQueue.begin(), m_writeQueue.end(),
                        [type, &id](const auto& request) {
                                return request.type == type && request.id == id;
                        });
                if (queued != m_writeQueue.end())
                        queued->image = std::move(image);
                else
                        m_writeQueue.push_back({type, id, std::move(image)});
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
        }
}

std::filesystem::path NfDiskCache::entryPath(CacheType type, const NfPhotoId& id) const
{
        const auto name = std::to_string(id.value()) + ".nfc";
        const auto typeDirectory = type == CacheType::Thumbnail ? "thumbnails" : "previews";
        return m_settings.directory / typeDirectory / name;
}

std::unique_ptr<NfImage> NfDiskCache::get(CacheType type, const NfPhotoId& id)
{
        std::lock_guard lock(m_mutex);
        const auto path = entryPath(type, id);
        std::error_code error;
        const auto fileSize = std::filesystem::file_size(path, error);
        if (error)
                return nullptr;

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
            || width > 100000 || height > 100000
            || channels == 0 || channels > 4
            || imageFormat > static_cast<std::uint32_t>(
                                     NfImageData::ImageFormat::Format_ARGB32_Premultiplied)
            || dataSize != static_cast<std::uint64_t>(width) * height * channels
            || fileSize != headerSize + dataSize
            || dataSize > m_settings.maxSizeBytes) {
                file.close();
                std::filesystem::remove(path, error);
                NF_LOG_ERROR("discarding invalid disk cache entry: " << path);
                return nullptr;
        }

        std::vector<unsigned char> pixels(static_cast<std::size_t>(dataSize));
        file.read(reinterpret_cast<char*>(pixels.data()),
                  static_cast<std::streamsize>(pixels.size()));
        if (!file) {
                file.close();
                std::filesystem::remove(path, error);
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
        std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(),
                                         error);
        return std::make_unique<NfImage>(std::move(data));
}

bool NfDiskCache::put(CacheType type, const NfPhotoId& id, const NfImage& image)
{
        const auto* data = image.getData();
        if (!data || data->empty() || data->width() <= 0 || data->height() <= 0
            || data->channels() <= 0 || data->channels() > 4
            || data->size() != static_cast<std::uint64_t>(data->width())
                    * data->height() * data->channels())
                return false;

        const auto fileSize = headerSize + data->size();
        if (fileSize > m_settings.maxSizeBytes)
                return false;

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
        if (error || space.available < m_settings.minimumFreeSpaceBytes
            || fileSize > space.available - m_settings.minimumFreeSpaceBytes) {
                NF_LOG_ERROR("disk cache write skipped because free space is below threshold: "
                               << destination.parent_path());
                return false;
        }

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

        if (!evictToFit(fileSize, destination)) {
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
                NF_LOG_ERROR("can't install disk cache entry: " << destination);
                return false;
        }
        return true;
}

bool NfDiskCache::evictToFit(std::uint64_t requiredBytes,
                            const std::filesystem::path& protectedPath)
{
        if (requiredBytes > m_settings.maxSizeBytes)
                return false;

        std::vector<CacheFile> files;
        std::uint64_t totalBytes = 0;
        std::error_code error;
        if (!std::filesystem::exists(m_settings.directory, error))
                return !error;

        for (std::filesystem::recursive_directory_iterator it(m_settings.directory, error), end;
             !error && it != end; it.increment(error)) {
                if (!it->is_regular_file(error) || error || it->path() == protectedPath
                    || it->path().extension() != ".nfc")
                        continue;
                const auto size = it->file_size(error);
                if (error)
                        continue;
                const auto modified = it->last_write_time(error);
                if (error)
                        continue;
                files.push_back({it->path(), size, modified});
                totalBytes += size;
        }
        if (error)
                return false;

        std::sort(files.begin(), files.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.modified < rhs.modified;
        });
        for (const auto& file : files) {
                if (totalBytes <= m_settings.maxSizeBytes - requiredBytes)
                        break;
                if (std::filesystem::remove(file.path, error) && !error)
                        totalBytes -= file.size;
                error.clear();
        }
        return totalBytes <= m_settings.maxSizeBytes - requiredBytes;
}

} // namespace NfCore
