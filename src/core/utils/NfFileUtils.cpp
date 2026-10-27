/**
 * File name: NfFileUtils.cpp
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

#include "NfFileUtils.h"
#include "NfStringUtils.h"

#include <utility>

namespace NfCore {

std::filesystem::path NfFileUtils::pathFromUtf8(std::string_view path)
{
        if (path.empty())
                return {};

        const auto* utf8Path = reinterpret_cast<const char8_t*>(path.data());
        return std::filesystem::path{std::u8string{utf8Path, path.size()}};
}

std::string NfFileUtils::pathToUtf8(const std::filesystem::path& path)
{
        const auto utf8Path = path.u8string();
        return {reinterpret_cast<const char*>(utf8Path.data()), utf8Path.size()};
}

std::string NfFileUtils::extension(const std::filesystem::path& path, Case casing)
{
        auto result = path.extension().string();
        if (!result.empty() && result.front() == '.')
                result.erase(0, 1);

        switch (casing) {
        case Case::Upper:
                return NfStringUtils::toUpper(std::move(result));
        case Case::Lower:
                return NfStringUtils::toLower(std::move(result));
        case Case::Preserve:
                return result;
        }

        return result;
}

bool NfFileUtils::fileExists(const std::filesystem::path& path)
{
        std::error_code ec;
        return std::filesystem::is_regular_file(path, ec);
}

std::optional<std::uintmax_t> NfFileUtils::fileSize(const std::filesystem::path& path)
{
        std::error_code error;
        const auto size = std::filesystem::file_size(path, error);

        if (error)
                return std::nullopt;

        return size;
}

} // namespace NfCore
