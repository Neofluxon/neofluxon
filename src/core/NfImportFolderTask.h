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
 */

#ifndef NF_IMPORT_FOLDER_TASK_H
#define NF_IMPORT_FOLDER_TASK_H

#include "NfImportFolderTaskData.h"
#include "NfUserTask.h"

#include <filesystem>
#include <memory>

namespace NfCore {

class NfLibrary;
class NfUserTaskManager;

class NfImportFolderTask : public NfUserTask {
public:
        NfImportFolderTask(const std::filesystem::path& folderPath,
                           std::unique_ptr<NfLibrary> library,
                           NfUserTaskManager* taskManager);
        NfImportFolderTask(NfImportFolderTask&&) noexcept = default;
        NfImportFolderTask& operator=(NfImportFolderTask&&) noexcept = default;
        NfImportFolderTask(const NfImportFolderTask&) = delete;
        NfImportFolderTask& operator=(const NfImportFolderTask&) = delete;
        ~NfImportFolderTask() override;

        void reportSubmitted();
        TaskStatus execute() override;

private:
        TaskStatus importFolder();
        TaskStatus importPhotosFromDirectory(const std::filesystem::path& path);
        void reportUpdate();

        std::filesystem::path m_path;
        std::unique_ptr<NfLibrary> m_library;
        NfImportFolderTaskData m_data;
};

} // namespace NfCore

#endif // NF_IMPORT_FOLDER_TASK_H
