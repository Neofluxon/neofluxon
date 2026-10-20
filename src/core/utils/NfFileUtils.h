/**
 * File name: NfFileUtils.h
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

#ifndef NF_FILE_UTILS_H
#define NF_FILE_UTILS_H

#include <filesystem>
#include <string>
#include <string_view>

namespace NfCore {

class NfFileUtils {
public:
        enum class Case {
                Preserve,
                Upper,
                Lower
        };

        NfFileUtils() = delete;
        static std::filesystem::path pathFromUtf8(std::string_view path);
        static std::string pathToUtf8(const std::filesystem::path& path);
        static std::string extension(const std::filesystem::path& path,
                                     Case casing = Case::Preserve);
};

} // namespace NfCore

#endif // NF_FILE_UTILS_H
