/**
 * File name: NfImportFolderTaskData.h
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

#ifndef NF_IMPORT_FOLDER_TASK_DATA_H
#define NF_IMPORT_FOLDER_TASK_DATA_H

#include <cstdint>
#include <filesystem>

namespace NfCore {

struct NfImportFolderTaskData {
        enum class Status {
                Scanning,
                Importing,
                Completed,
                Failed,
                Cancelled
        };

        std::filesystem::path sourceFolder;
        uint64_t discoveredPhotos{0};
        uint64_t processedPhotos{0};
        uint64_t importedPhotos{0};
        uint64_t skippedPhotos{0};
        uint64_t failedPhotos{0};
        Status status{Status::Scanning};
};

} // namespace NfCore

#endif // NF_IMPORT_FOLDER_TASK_DATA_H
