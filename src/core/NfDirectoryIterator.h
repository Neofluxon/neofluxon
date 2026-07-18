/**
 * File name: NfPhotoDirectoryIterator.h
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

#ifndef NF_DIRECTORY_ITERATOR_H
#define NF_DIRECTORY_ITERATOR_H

#include <filesystem>
#include <optional>

namespace NfCore {

class NfDirectoryIterator
{
public:
        NfDirectoryIterator();

        void setPath(const std::filesystem::path& path,
                     bool recursive = true);

        void setRecursive(bool recursive);

        std::optional<std::filesystem::path> next();

private:
        bool m_recursive{true};
        std::filesystem::path m_path;
        std::filesystem::directory_iterator m_dirIterator;
        std::filesystem::directory_iterator m_dirEnd;
        std::filesystem::recursive_directory_iterator m_recursiveIterator;
        std::filesystem::recursive_directory_iterator m_recursiveEnd;
};

} // namespace NfCore

#endif // NF_DIRECTORY_ITERATOR_H
