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

#include "NfLibraryFolderImportTask.h"
#include "NfLibrary.h"
#include "NfPhoto.h"
#include "NfDirectoryIterator.h"
#include "NfPhotoDirectoryIterator.h"
#include "NfPhotoMetadataExtractor.h"
#include "NfPhotoSummary.h"
#include "NfLogger.h"

namespace NfCore {

NfLibraryFolderImportTask::NfLibraryFolderImportTask(const std::filesystem::path &folderPath,
                                                     std::unique_ptr<NfLibrary> library)
        : m_path{folderPath}
        , m_library{std::move(library)}
{
}

NfLibraryFolderImportTask::~NfLibraryFolderImportTask() = default;

NfTask::TaskStatus NfLibraryFolderImportTask::execute()
{
        NF_LOG_DEBUG("scan folder: " << m_path);

        if (m_library->folderExists(m_path)) {
                NF_LOG_DEBUG("folder already exists, skip:" << m_path);
                return TaskStatus::Success;
        }

        NfDirectoryIterator dirIterator;
        dirIterator.setPath(m_path);

        // Import photos from the root path.
        importPhotosFromDirectory(m_path);

        while (auto dir = dirIterator.next()) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                if (m_library->folderExists(*dir)) {
                        NF_LOG_DEBUG("folder already exists, skip:" << *dir);
                        continue;
                }

                importPhotosFromDirectory(*dir);
        }

        NF_LOG_DEBUG("folder import finished: " << m_path);

        return TaskStatus::Success;
}

void NfLibraryFolderImportTask::importPhotosFromDirectory(const std::filesystem::path& path)
{
        NF_LOG_DEBUG("import photos from path: " << path);

        m_library->addFolder(path);

        NfPhotoDirectoryIterator photoIterator;
        photoIterator.setPath(path, false);

        while (auto photo = photoIterator.next()) {
                if (isCancelled())
                        return;

                m_library->addPhoto(*photo);
        }
}

} // namespace NfCore
