/**
 * File name: NfImportFolderTask.h
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

#include "NfImportFolderTask.h"
#include "NfDirectoryIterator.h"
#include "NfPhotoDirectoryIterator.h"
#include "NfUserTaskManager.h"
#include "NfUserTaskData.h"
#include "library/NfLibrary.h"
#include "NfPhoto.h"
#include "NfLogger.h"

#include <chrono>
#include <utility>

namespace NfCore {

NfImportFolderTask::NfImportFolderTask(const std::filesystem::path& folderPath,
                                       std::unique_ptr<NfLibrary> library,
                                       NfUserTaskManager* taskManager)
        : NfUserTask{taskManager}
        , m_path{folderPath}
        , m_library{std::move(library)}
        , m_progressTimer{std::chrono::milliseconds{500},
                          [this]() { notifyUpdated(); }}
{
        m_data.sourceFolder = folderPath;
}

NfImportFolderTask::~NfImportFolderTask() = default;

const std::filesystem::path& NfImportFolderTask::path() const
{
        return m_path;
}

NfTask::TaskStatus NfImportFolderTask::execute()
{
        const auto result = importFolder();
        switch (result) {
        case TaskStatus::Success:
                m_data.status = NfImportFolderTaskData::Status::Completed;
                break;
        case TaskStatus::Failed:
                m_data.status = NfImportFolderTaskData::Status::Failed;
                break;
        case TaskStatus::Cancelled:
                m_data.status = NfImportFolderTaskData::Status::Cancelled;
                break;
        }

        notifyUpdated();

        return result;
}

void NfImportFolderTask::notifySubmitted()
{
        notifyTaskAdded(m_data);
}

void NfImportFolderTask::notifyCanceled()
{
        // TODO: for now is canceled is qualified as removed too
        // Implement cancel nofication for UI
        notifyRemoved();
}

void NfImportFolderTask::notifyUpdated()
{
        notifyTaskUpdated(m_data);
}

void NfImportFolderTask::notifyRemoved()
{
        notifyTaskRemoved();
}

NfTask::TaskStatus NfImportFolderTask::importFolder()
{
        NF_LOG_DEBUG("scan folder: " << m_path);

        m_data.status = NfImportFolderTaskData::Status::Importing;
        notifyUpdated();

        // Check if the folder already has been imported
        // TODO: probably in the future we will ignore this check
        // so we can reimport maybe new added photos in the path
        if (m_library->folderExists(m_path)) {
                m_data.status = NfImportFolderTaskData::Status::Completed;
                NF_LOG_DEBUG("folder already exists, skip:" << m_path);
                return TaskStatus::Success;
        }

        NfDirectoryIterator dirIterator;
        if (const auto result = dirIterator.setPath(m_path); !result) {
                NF_LOG_ERROR("failed to scan folder " << m_path << ": "
                             << result.error().what());
                return TaskStatus::Failed;
        }

         // First import photos from the root directory.
        if (const auto status = importPhotosFromDirectory(m_path);
            status != TaskStatus::Success) {
                return status;
        }

        // Import photos from the directory tree.
        while (true) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                auto nextDirectory = dirIterator.next();
                if (!nextDirectory) {
                        NF_LOG_ERROR("failed to scan folder " << m_path << ": "
                                     << nextDirectory.error().what());
                        return TaskStatus::Failed;
                }

                // No more directories. Import finished.
                if (!nextDirectory.value()) {
                        break;
                }

                auto directory = *nextDirectory.value();
                if (m_library->folderExists(directory)) {
                        NF_LOG_DEBUG("folder already exists, skip:" << directory);
                        continue;
                }

                if (const auto status = importPhotosFromDirectory(directory);
                    status != TaskStatus::Success)
                        return status;

                notifyUpdated();
        }

        NF_LOG_DEBUG("folder import finished: " << m_path);

        return TaskStatus::Success;
}

NfTask::TaskStatus
NfImportFolderTask::importPhotosFromDirectory(const std::filesystem::path& path)
{
        NF_LOG_DEBUG("import photos from directory: " << path);

        // TODO: probably in the future we will check the status to see
        // if the fail reson is that folder exist in order to allow reimport
        // of new photos.
        if (!m_library->addFolder(path)) {
                NF_LOG_ERROR("Failed add folder to library:" << path);
                return TaskStatus::Failed;
        }

        NfPhotoDirectoryIterator photoIterator;
        if (const auto result = photoIterator.setPath(path, false); !result) {
                NF_LOG_ERROR("failed to scan photos in folder " << path << ": "
                             << result.error().what());
                return TaskStatus::Failed;
        }

        while (true) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                auto nextPhoto = photoIterator.next();
                if (!nextPhoto) {
                        NF_LOG_ERROR("failed to scan photos in folder "
                                     << path << ": "
                                     << nextPhoto.error().what());
                        return TaskStatus::Failed;
                }

                // No more photos found return success.
                if (!nextPhoto.value())
                        return TaskStatus::Success;

                m_data.discoveredPhotos++;

                auto& photo = *nextPhoto.value();
                if (!photo.loadSummary()) {
                        NF_LOG_ERROR("failed to load photo summary: "
                                     << photo.path());
                        m_data.failedPhotos++;
                        m_progressTimer.tick();
                        continue;
                }

                m_library->addPhoto(photo);
                m_data.importedPhotos++;

                m_progressTimer.tick();
        }
}

} // namespace NfCore
