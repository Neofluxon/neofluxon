/**
 * File name: NfDirectoryIterator.cpp
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

#include "NfDirectoryIterator.h"

namespace NfCore {

NfDirectoryIterator::NfDirectoryIterator()
{
}

void NfDirectoryIterator::setPath(const std::filesystem::path& path,
                                  bool recursive)
{
        m_path = path;
        m_recursive = recursive;

        if (m_recursive)
                m_recursiveIterator = std::filesystem::recursive_directory_iterator(m_path);
        else
                m_dirIterator = std::filesystem::directory_iterator(m_path);
}

void NfDirectoryIterator::setRecursive(bool recursive)
{
        m_recursive = recursive;
}

std::optional<std::filesystem::path> NfDirectoryIterator::next()
{
        if (m_recursive) {
                while (m_recursiveIterator != m_recursiveEnd) {
                        auto entry = *m_recursiveIterator;
                        ++m_recursiveIterator;

                        if (entry.is_directory())
                                return entry.path();
                }
        } else {
                while (m_dirIterator != m_dirEnd) {
                        auto entry = *m_dirIterator;
                        ++m_dirIterator;

                        if (entry.is_directory())
                                return entry.path();
                }
        }

        return std::nullopt;
}

} // namespace NfCore
