/**
 * File name: NfLibraryFolderImportTask.h
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

#ifndef NF_LIBRARY_IMPORT_TASK_H
#define NF_LIBRARY_IMPORT_TASK_H

#include "NfTask.h"

#include <memory>
#include <filesystem>

namespace NfCore {

class NfLibrary;

class NfLibraryFolderImportTask : public NfTask {
public:
        NfLibraryFolderImportTask(const std::filesystem::path &folderPath,
                                  std::unique_ptr<NfLibrary> library);
        NfLibraryFolderImportTask(NfLibraryFolderImportTask&&) noexcept = default;
        NfLibraryFolderImportTask& operator=(NfLibraryFolderImportTask&&) noexcept = default;
        NfLibraryFolderImportTask(const NfLibraryFolderImportTask&) = delete;
        NfLibraryFolderImportTask& operator=(const NfLibraryFolderImportTask&) = delete;
        ~NfLibraryFolderImportTask();
        NfTask::TaskStatus execute() override;

private:
        std::filesystem::path m_path;
        std::unique_ptr<NfLibrary> m_library;
};

} // namespace NfCore

#endif // NF_LIBRARY_IMPORT_TASK_H
