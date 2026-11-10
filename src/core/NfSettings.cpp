/**
 * File name: NfSettings.cpp
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
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#include "NfSettings.h"
#include "NfApplicationPaths.h"
#include "NfSupportedFormats.h"

#include <INIReader.h>

#include <charconv>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace NfCore {

namespace {

constexpr auto settingsFileName = "setting.ini";

NfCacheMemoryAllocation splitMemoryBudget(std::uint64_t totalBytes,
                                          std::uint64_t pixmapPercent)
{
        const auto pixmapBytes = totalBytes / 100 * pixmapPercent
                + totalBytes % 100 * pixmapPercent / 100;
        return {
                .pixmapBytes = pixmapBytes,
                .decodedBytes = totalBytes - pixmapBytes
        };
}

std::filesystem::path settingsPath(const char* fileName)
{
        return NfApplicationPaths().configDirectory() / fileName;
}

template<typename T>
bool parseUnsigned(const INIReader& reader,
                   const std::string& section,
                   const std::string& key,
                   T& value)
{
        if (!reader.HasValue(section, key))
                return false;
        const auto text = reader.Get(section, key, {});
        if (text.empty())
                return false;
        const auto [end, error] = std::from_chars(
                text.data(), text.data() + text.size(), value);
        return error == std::errc{} && end == text.data() + text.size();
}

bool parseQuotedString(const INIReader& reader,
                       const std::string& section,
                       const std::string& key,
                       std::string& value)
{
        if (!reader.HasValue(section, key))
                return false;

        std::istringstream stream(reader.Get(section, key, {}));
        if (!(stream >> std::quoted(value)))
                return false;
        stream >> std::ws;

        return stream.eof();
}

bool isSupportedFormat(const std::string& extension)
{
        return NfSupportedFormats::all.contains(extension);
}

} // namespace

NfSettings NfSettings::defaults()
{
        NfSettings setting;
        setting.diskCacheDirectory = NfApplicationPaths().cacheDirectory() / "images";
        for (const auto& format : NfSupportedFormats::all)
                setting.selectedFormats.emplace(format.first);
        return setting;
}

NfCacheMemoryAllocation NfSettings::thumbnailMemoryAllocation() const
{
        return splitMemoryBudget(thumbnailMemoryLimitBytes, 30);
}

NfCacheMemoryAllocation NfSettings::previewMemoryAllocation() const
{
        return splitMemoryBudget(previewMemoryLimitBytes, 15);
}

NfSettings NfSettings::load(std::string* error)
{
        if (error)
                error->clear();

        auto setting = defaults();
        const auto path = settingsPath(settingsFileName);
        std::error_code filesystemError;
        const bool exists = std::filesystem::exists(path, filesystemError);
        if (filesystemError) {
                if (error)
                        *error = "Unable to check application settings: "
                                + filesystemError.message();
                return setting;
        }

        if (!exists)
                return setting;

        INIReader ini(path.string());
        if (ini.ParseError() != 0) {
                if (error)
                        *error = ini.ParseError() == -1
                                ? "Unable to open application settings."
                                : "Invalid application settings at line "
                                + std::to_string(ini.ParseError()) + ".";
                return setting;
        }

        std::uint64_t selectedFormatCount{};
        std::string directory;
        std::uint64_t maxSizeBytes{};
        std::uint64_t minimumFreeSpaceBytes{};
        std::uint64_t thumbnailMemoryLimitBytes{};
        std::uint64_t previewMemoryLimitBytes{};
        if (!parseQuotedString(ini, "Cache", "directory", directory)
            || directory.empty()
            || !parseUnsigned(ini, "Cache", "disk_limit_bytes", maxSizeBytes)
            || maxSizeBytes == 0
            || !parseUnsigned(ini, "Cache", "minimum_free_space_bytes",
                              minimumFreeSpaceBytes)
            || !parseUnsigned(ini, "Cache.Thumbnails", "total_budget_bytes",
                              thumbnailMemoryLimitBytes)
            || thumbnailMemoryLimitBytes == 0
            || !parseUnsigned(ini, "Cache.Previews", "total_budget_bytes",
                              previewMemoryLimitBytes)
            || previewMemoryLimitBytes == 0
            || !parseUnsigned(ini, "SupportedExtensions", "count",
                              selectedFormatCount)
            || selectedFormatCount > NfSupportedFormats::all.size()) {
                if (error)
                        *error = "Invalid application settings; using defaults.";
                return setting;
        }

        std::set<std::string> selectedFormats;
        for (std::uint64_t i = 1; i <= selectedFormatCount; ++i) {
                const auto extensionKey = "ext" + std::to_string(i);
                if (!ini.HasValue("SupportedExtensions", extensionKey)) {
                        if (error)
                                *error = "Invalid application settings; using defaults.";
                        return setting;
                }
                const auto extensionValue = ini.Get(
                        "SupportedExtensions", extensionKey, {});
                if (isSupportedFormat(extensionValue))
                        selectedFormats.insert(extensionValue);
        }

        setting.diskCacheDirectory = std::filesystem::path{directory};
        setting.diskCacheMaxSizeBytes = maxSizeBytes;
        setting.minimumFreeDiskSpaceBytes = minimumFreeSpaceBytes;
        setting.thumbnailMemoryLimitBytes = thumbnailMemoryLimitBytes;
        setting.previewMemoryLimitBytes = previewMemoryLimitBytes;
        setting.selectedFormats = std::move(selectedFormats);

        return setting;
}

bool NfSettings::save(std::string& error) const
{
        if (diskCacheDirectory.empty() || diskCacheMaxSizeBytes == 0
            || thumbnailMemoryLimitBytes == 0 || previewMemoryLimitBytes == 0) {
                error = "Cache directory and cache limits must be set.";
                return false;
        }

        const auto path = settingsPath(settingsFileName);
        std::error_code filesystemError;
        std::filesystem::create_directories(path.parent_path(), filesystemError);
        if (filesystemError) {
                error = "Unable to create settings directory: "
                        + filesystemError.message();
                return false;
        }

        const auto temporaryPath = path.string() + ".tmp";
        std::size_t supportedFormatCount{};
        for (const auto& extension : selectedFormats) {
                if (isSupportedFormat(extension))
                        ++supportedFormatCount;
        }

        {
                std::ofstream file(temporaryPath, std::ios::trunc);
                if (!file) {
                        error = "Unable to write application settings.";
                        return false;
                }

                file << "[Cache]\n"
                     << "directory=" << std::quoted(diskCacheDirectory.string()) << '\n'
                     << "disk_limit_bytes=" << diskCacheMaxSizeBytes << '\n'
                     << "minimum_free_space_bytes=" << minimumFreeDiskSpaceBytes
                     << "\n\n"
                     << "[Cache.Thumbnails]\n"
                     << "total_budget_bytes=" << thumbnailMemoryLimitBytes
                     << "\n\n"
                     << "[Cache.Previews]\n"
                     << "total_budget_bytes=" << previewMemoryLimitBytes
                     << "\n\n"
                     << "[SupportedExtensions]\n"
                     << "count=" << supportedFormatCount << '\n';

                std::size_t index{};
                for (const auto& extension : selectedFormats) {
                        if (isSupportedFormat(extension))
                                file << "ext" << ++index << '=' << extension << '\n';
                }

                file.flush();

                if (!file) {
                        error = "Unable to write application settings.";
                        return false;
                }
        }

        std::filesystem::rename(temporaryPath, path, filesystemError);
        if (filesystemError) {
                std::filesystem::remove(temporaryPath);
                error = "Unable to save application settings: "
                        + filesystemError.message();
                return false;
        }

        error.clear();

        return true;
}

} // namespace NfCore
