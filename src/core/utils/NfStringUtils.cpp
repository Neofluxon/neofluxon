/**
 * File name: NfStringUtils.cpp
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

#include "NfStringUtils.h"

#include <algorithm>
#include <cctype>
#include <ranges>

namespace NfCore {

std::string NfStringUtils::toUpper(std::string value)
{
        std::ranges::transform(value, value.begin(),
                               [](auto character) {
                                       return static_cast<char>(
                                               std::toupper(static_cast<unsigned char>(character)));
                               });

        return value;
}

std::string NfStringUtils::toLower(std::string value)
{
        std::ranges::transform(value, value.begin(),
                               [](auto character) {
                                       return static_cast<char>(
                                               std::tolower(static_cast<unsigned char>(character)));
                               });

        return value;
}

bool NfStringUtils::compareNoCase(std::string_view str1, std::string_view str2)
{
        return std::ranges::equal(str1, str2, [](auto c1, auto c2) {
                return std::tolower(static_cast<unsigned char>(c1)) ==
                       std::tolower(static_cast<unsigned char>(c2));
        });
}

std::string NfStringUtils::trim(std::string value)
{
        const auto isSpace = [](unsigned char character) {
                return std::isspace(character);
        };

        const auto first = std::ranges::find_if_not(value, isSpace);
        const auto last = std::ranges::find_if_not(value | std::views::reverse, isSpace).base();
        if (first >= last)
                return {};

        return std::string(first, last);
}

std::string NfStringUtils::removePrefix(std::string value,
                                        std::string_view prefix,
                                        bool ignoreCase)
{
        if (prefix.size() > value.size())
                return value;

        const auto valuePrefix = std::string_view(value).substr(0, prefix.size());
        if (ignoreCase ? !compareNoCase(valuePrefix, prefix) : valuePrefix != prefix)
                return value;

        value.erase(0, prefix.size());

        return value;
}

} // namespace NfCore
