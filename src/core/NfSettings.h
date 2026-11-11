/**
 * File name: NfSettings.h
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

#ifndef NF_SETTINGS_H
#define NF_SETTINGS_H

#include <cstdint>
#include <filesystem>
#include <set>
#include <string>

namespace NfCore {

struct NfCacheMemoryAllocation
{
        std::uint64_t pixmapBytes{};
        std::uint64_t decodedBytes{};
};

struct NfSettings
{
        std::filesystem::path diskCacheDirectory;
        std::uint64_t diskCacheMaxSizeBytes{20ULL * 1024 * 1024 * 1024};
        std::uint64_t minimumFreeDiskSpaceBytes{10ULL * 1024 * 1024 * 1024};
        std::uint64_t thumbnailMemoryLimitBytes{512ULL * 1024 * 1024};
        std::uint64_t previewMemoryLimitBytes{1024ULL * 1024 * 1024};
        std::set<std::string> selectedFormats;

        static NfSettings defaults();
        static NfSettings load(std::string* error = nullptr);
        bool save(std::string& error) const;
        NfCacheMemoryAllocation thumbnailMemoryAllocation() const;
        NfCacheMemoryAllocation previewMemoryAllocation() const;
};

} // namespace NfCore

#endif // NF_SETTINGS_H
