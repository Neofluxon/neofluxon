/**
 * File name: NfPhotoId.h
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

#ifndef NF_PHOTO_ID_H
#define NF_PHOTO_ID_H

#include <filesystem>
#include <cstdint>

namespace NfCore {

class NfPhotoId {
public:
        explicit NfPhotoId(const std::filesystem::path& filePath = {});

        bool operator==(const NfPhotoId& other) const;
        bool operator!=(const NfPhotoId& other) const;

        explicit operator bool() const { return isValid(); }
        bool isValid() const;

        uint64_t value() const { return m_idHash; }
        static uint64_t computeHash(const std::filesystem::path& filePath);

private:
        uint64_t m_idHash;
};

} // namespace NfCore

namespace std {
    template <>
    struct hash<NfCore::NfPhotoId> {
        size_t operator()(const NfCore::NfPhotoId& id) const noexcept {
            return static_cast<size_t>(id.value());
        }
    };
}

#endif // NF_PHOTO_ID_H
