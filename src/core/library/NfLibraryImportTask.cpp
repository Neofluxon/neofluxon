/**
 * File name: NfLibraryImportTask.cpp
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

#include "NfLibraryImportTask.h"
#include "NfLibrary.h"
#include "NfPhotoDirectoryIterator.h"

namespace NfCore {

NfLibraryFolderImport::NfLibraryFolderImport(const std::filesystem::path &folderPath,
                                             NfLibrary* library)
        : m_path{path},
          m_library{library}
{
}

NfLibraryFolderImport::~NfLibraryFolderImport() = default;

TaskStatus NfLibraryFolderImport::execute()
{
        NfPhotoDirectoryIterator iterator;
        iterator.setPath(path());

        while (auto photo = iterator.next()) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                m_library->addPhoto(photo);
        }

        return TaskStatus::Done;
}

} // namespace NfCore

