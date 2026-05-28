/**
 * File name: NfPhotoDirectoryIterator.cpp
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

#include "NfPhotoDirectoryIterator.h"
#include "NfPhoto.h"

#include <algorithm>

namespace NfCore {

NfPhotoDirectoryIterator::NfPhotoDirectoryIterator()
{
        setExtensions({
                        ".cr3",
                        ".cr2",
                        ".dng",
                        ".raf",
                        ".nef",
                        ".jpg",
                        ".jpeg",
                        ".png"
                });
}

void NfPhotoDirectoryIterator::setPath(const std::filesystem::path& path,
                                       bool recursive)
{
        namespace fs = std::filesystem;

        m_path = path;
        m_recursive = recursive;
        try {
                if (m_recursive) {
                        m_recursiveIterator = fs::recursive_directory_iterator(m_path);
                        m_recursiveEnd = fs::recursive_directory_iterator();

                } else {
                        m_dirIterator = fs::directory_iterator(m_path);
                        m_dirEnd = fs::directory_iterator();
                }
        }
        catch (const fs::filesystem_error&) {
        }
}

void NfPhotoDirectoryIterator::setRecursive(bool recursive)
{
        m_recursive = recursive;
}

void NfPhotoDirectoryIterator::setExtensions(const std::vector<std::string>& extensions)
{
        m_extensions.clear();
        for (const auto& ext : extensions)
                addExtension(ext);
}

void NfPhotoDirectoryIterator::addExtension(const std::string& extension)
{
        m_extensions.insert(normalizeExtension(extension));
}

std::string NfPhotoDirectoryIterator::normalizeExtension(const std::string& extension) const
{
        std::string ext = extension;
        std::transform(ext.begin(),
                       ext.end(),
                       ext.begin(),
                       [](unsigned char c) {
                               return std::tolower(c);
                       });

        if (!ext.empty() && ext[0] != '.')
                ext.insert(ext.begin(), '.');

        return ext;
}

bool NfPhotoDirectoryIterator::isSupportedExtension(const std::filesystem::path& path) const
{
        namespace fs = std::filesystem;

        try {
                if (!fs::is_regular_file(path))
                        return false;

                auto ext = normalizeExtension(path.extension().string());
                return m_extensions.contains(ext);
        }
        catch (const fs::filesystem_error&) {
                return false;
        }
}

std::optional<NfPhoto> NfPhotoDirectoryIterator::next()
{
        namespace fs = std::filesystem;

        try {
                while (true) {
                        fs::path path;
                        if (m_recursive) {
                                if (m_recursiveIterator == m_recursiveEnd)
                                        return std::nullopt;
                                path = m_recursiveIterator->path();
                                ++m_recursiveIterator;
                        } else {
                                if (m_dirIterator == m_dirEnd)
                                        return std::nullopt;
                                path = m_dirIterator->path();
                                ++m_dirIterator;
                        }

                        if (!isSupportedExtension(path))
                                continue;

                        return NfPhoto(path);
                }
        }
        catch (const fs::filesystem_error&) {
                return std::nullopt;
        }
}

} // namespace NfCore
