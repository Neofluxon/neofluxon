/**
 * File name: NfDiskCacheSettings.cpp
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

#include "NfDiskCacheSettings.h"
#include "NfApplicationPaths.h"

#include <fstream>
#include <iomanip>
#include <limits>

namespace NfCore {

namespace {

std::filesystem::path settingsPath()
{
        return NfApplicationPaths().configDirectory() / "disk-cache.ini";
}

} // namespace

NfDiskCacheSettings NfDiskCacheSettings::defaults()
{
        NfDiskCacheSettings settings;
        settings.directory = NfApplicationPaths().cacheDirectory() / "images";
        return settings;
}

NfDiskCacheSettings NfDiskCacheSettings::load(std::string* error)
{
        auto settings = defaults();
        std::ifstream file(settingsPath());
        if (!file)
                return settings;

        std::string header;
        std::string directory;
        std::uint64_t maxSizeBytes{};
        std::uint64_t minimumFreeSpaceBytes{};
        if (!(file >> header >> std::quoted(directory)
              >> maxSizeBytes >> minimumFreeSpaceBytes)
            || header != "NeofluxonDiskCache1"
            || directory.empty()
            || maxSizeBytes == 0) {
                if (error)
                        *error = "Invalid disk cache settings; using defaults.";
                return settings;
        }

        settings.directory = std::filesystem::path{directory};
        settings.maxSizeBytes = maxSizeBytes;
        settings.minimumFreeSpaceBytes = minimumFreeSpaceBytes;
        return settings;
}

bool NfDiskCacheSettings::save(std::string& error) const
{
        if (directory.empty() || maxSizeBytes == 0) {
                error = "Disk cache directory and size limit must be set.";
                return false;
        }

        const auto path = settingsPath();
        std::error_code filesystemError;
        std::filesystem::create_directories(path.parent_path(), filesystemError);
        if (filesystemError) {
                error = "Unable to create settings directory: " + filesystemError.message();
                return false;
        }

        const auto temporaryPath = path.string() + ".tmp";
        {
                std::ofstream file(temporaryPath, std::ios::trunc);
                if (!file) {
                        error = "Unable to write disk cache settings.";
                        return false;
                }
                file << "NeofluxonDiskCache1\n"
                     << std::quoted(directory.string()) << '\n'
                     << maxSizeBytes << '\n'
                     << minimumFreeSpaceBytes << '\n';
                file.flush();
                if (!file) {
                        error = "Unable to write disk cache settings.";
                        return false;
                }
        }

        std::filesystem::rename(temporaryPath, path, filesystemError);
        if (filesystemError) {
                std::filesystem::remove(temporaryPath);
                error = "Unable to save disk cache settings: " + filesystemError.message();
                return false;
        }

        error.clear();
        return true;
}

} // namespace NfCore
