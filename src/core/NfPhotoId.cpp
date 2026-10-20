/**
 * File name: NfPhotoId.cpp
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "NfPhotoId.h"
#include "utils/NfFileUtils.h"

#include <string>
#include <chrono>

namespace NfCore {

namespace {
uint64_t fnv1a_64(const std::string& data)
{
        const uint64_t FNV_offset = 14695981039346656037ULL;
        const uint64_t FNV_prime  = 1099511628211ULL;

        uint64_t hash = FNV_offset;
        for (unsigned char c : data) {
                hash ^= c;
                hash *= FNV_prime;
        }

        return hash;
}
} // snamespace

NfPhotoId::NfPhotoId(const std::filesystem::path& filePath)
        : m_idHash{computeHash(filePath)}
{
}

bool NfPhotoId::operator==(const NfPhotoId& other) const
{
        return m_idHash == other.m_idHash;
}

bool NfPhotoId::operator!=(const NfPhotoId& other) const
{
        return !(*this == other);
}

bool NfPhotoId::isValid() const
{
        return m_idHash != 0;
}

uint64_t NfPhotoId::computeHash(const std::filesystem::path& filePath)
{
        try {
                if (!std::filesystem::exists(filePath))
                        return 0;

                auto size = std::filesystem::file_size(filePath);
                auto ftime = std::filesystem::last_write_time(filePath);
                auto sctp = std::chrono::system_clock::now() +
                        (ftime - decltype(ftime)::clock::now());
                auto time = std::chrono::system_clock::to_time_t(sctp);

                std::string key = NfFileUtils::pathToUtf8(filePath) + "|" +
                        std::to_string(size) + "|" +
                        std::to_string(time);

                return fnv1a_64(key);
        }
        catch (...) {
                return 0;
        }
}

} // NfCore
