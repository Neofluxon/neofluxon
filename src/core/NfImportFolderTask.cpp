/**
 * File name: NfImportFolderTask.cpp
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
 */

#include "NfImportFolderTask.h"

#include "NfDirectoryIterator.h"
#include "NfPhotoDirectoryIterator.h"
#include "NfUserTaskManager.h"
#include "NfUserTaskData.h"
#include "library/NfLibrary.h"
#include "NfPhoto.h"
#include "NfLogger.h"

#include <utility>

namespace NfCore {

NfImportFolderTask::NfImportFolderTask(const std::filesystem::path& folderPath,
                                       std::unique_ptr<NfLibrary> library,
                                       NfUserTaskManager* taskManager)
        : m_path{folderPath}
        , m_library{std::move(library)}
        , NfUserTask{taskManager}
{
        m_data.sourceFolder = folderPath;
}

NfImportFolderTask::~NfImportFolderTask() = default;

void NfImportFolderTask::reportSubmitted()
{
        publishTaskAdded({NfUserTaskType::ImportFolder, m_data});
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
        reportUpdate();
        return result;
}

NfTask::TaskStatus NfImportFolderTask::importFolder()
{
        NF_LOG_DEBUG("scan folder: " << m_path);

        NfDirectoryIterator dirIterator;
        if (const auto result = dirIterator.setPath(m_path); !result) {
                NF_LOG_ERROR("failed to scan folder " << m_path << ": "
                             << result.error().what());
                return TaskStatus::Failed;
        }

        NfPhotoDirectoryIterator photoCounter;
        if (const auto result = photoCounter.setPath(m_path); !result) {
                NF_LOG_ERROR("failed to count photos in folder " << m_path << ": "
                             << result.error().what());
                return TaskStatus::Failed;
        }

        while (true) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                auto nextPhoto = photoCounter.next();
                if (!nextPhoto) {
                        NF_LOG_ERROR("failed to count photos in folder " << m_path << ": "
                                     << nextPhoto.error().what());
                        return TaskStatus::Failed;
                }
                if (!nextPhoto.value())
                        break;

                ++m_data.discoveredPhotos;
                reportUpdate();
        }

        m_data.status = NfImportFolderTaskData::Status::Importing;
        reportUpdate();

        if (m_library->folderExists(m_path)) {
                m_data.processedPhotos = m_data.discoveredPhotos;
                m_data.skippedPhotos = m_data.discoveredPhotos;
                reportUpdate();
                NF_LOG_DEBUG("folder already exists, skip:" << m_path);
                return TaskStatus::Success;
        }

        if (const auto status = importPhotosFromDirectory(m_path);
            status != TaskStatus::Success)
                return status;

        while (true) {
                if (isCancelled())
                        return TaskStatus::Cancelled;

                auto nextDirectory = dirIterator.next();
                if (!nextDirectory) {
                        NF_LOG_ERROR("failed to scan folder " << m_path << ": "
                                     << nextDirectory.error().what());
                        return TaskStatus::Failed;
                }

                if (!nextDirectory.value())
                        break;

                auto directory = *nextDirectory.value();
                if (m_library->folderExists(directory)) {
                        NF_LOG_DEBUG("folder already exists, skip:" << directory);
                        continue;
                }

                if (const auto status = importPhotosFromDirectory(directory);
                    status != TaskStatus::Success)
                        return status;
        }

        if (m_data.processedPhotos < m_data.discoveredPhotos) {
                const auto skipped = m_data.discoveredPhotos - m_data.processedPhotos;
                m_data.processedPhotos += skipped;
                m_data.skippedPhotos += skipped;
                reportUpdate();
        }

        NF_LOG_DEBUG("folder import finished: " << m_path);
        return TaskStatus::Success;
}

NfTask::TaskStatus
NfImportFolderTask::importPhotosFromDirectory(const std::filesystem::path& path)
{
        NF_LOG_DEBUG("import photos from path: " << path);

        m_library->addFolder(path);

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
                        NF_LOG_ERROR("failed to scan photos in folder " << path << ": "
                                     << nextPhoto.error().what());
                        return TaskStatus::Failed;
                }
                if (!nextPhoto.value())
                        return TaskStatus::Success;

                auto& photo = *nextPhoto.value();
                if (!photo.loadSummary()) {
                        NF_LOG_ERROR("failed to load photo summary: " << photo.path());
                        ++m_data.processedPhotos;
                        ++m_data.failedPhotos;
                        reportUpdate();
                        continue;
                }

                m_library->addPhoto(photo);
                ++m_data.processedPhotos;
                ++m_data.importedPhotos;
                reportUpdate();
        }
}

void NfImportFolderTask::reportUpdate()
{
        publishTaskUpdated({NfUserTaskType::ImportFolder, m_data});
}

} // namespace NfCore
