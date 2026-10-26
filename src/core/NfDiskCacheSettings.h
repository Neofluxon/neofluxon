/**
 * File name: NfDiskCacheSettings.h
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

#ifndef NF_DISK_CACHE_SETTINGS_H
#define NF_DISK_CACHE_SETTINGS_H

#include <cstdint>
#include <filesystem>
#include <string>

namespace NfCore {

struct NfDiskCacheSettings {
        std::filesystem::path directory;
        std::uint64_t maxSizeBytes{50ULL * 1024 * 1024 * 1024};
        std::uint64_t minimumFreeSpaceBytes{10ULL * 1024 * 1024 * 1024};

        static NfDiskCacheSettings defaults();
        static NfDiskCacheSettings load(std::string* error = nullptr);
        bool save(std::string& error) const;
};

} // namespace NfCore

#endif // NF_DISK_CACHE_SETTINGS_H
