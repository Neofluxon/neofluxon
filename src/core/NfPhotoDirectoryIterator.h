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

#ifndef NF_PHOTO_DIRECTORY_ITERATOR_H
#define NF_PHOTO_DIRECTORY_ITERATOR_H

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace NfCore {

class NfPhoto;

class NfPhotoDirectoryIterator
{
public:
        NfPhotoDirectoryIterator();

        void setPath(const std::filesystem::path& path,
                     bool recursive = true);
        void setRecursive(bool recursive);
        void setExtensions(const std::vector<std::string>& extensions);
        void addExtension(const std::string& extension);
        std::optional<NfPhoto> next();

private:
        bool isSupportedExtension(const std::filesystem::path& path) const;
        std::string normalizeExtension(const std::string& extension) const;

private:
        bool m_recursive{true};
        std::filesystem::path m_path;
        std::filesystem::directory_iterator m_dirIterator;
        std::filesystem::directory_iterator m_dirEnd;
        std::filesystem::recursive_directory_iterator m_recursiveIterator;
        std::filesystem::recursive_directory_iterator m_recursiveEnd;
        std::unordered_set<std::string> m_extensions;
};

} //namespace NfCore

#endif // NF_PHOTO_DIRECTORY_ITERATOR_H

