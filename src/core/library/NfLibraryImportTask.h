/**
 * File name: NfLibraryImportTask.h
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

namespace NfCore {

class NfLibraryImportTask : public NfTask {
public:
        NfLibraryImportTask();
        NfLibraryImportTask(NfLibraryImportTask&&) noexcept = default;
        NfLibraryImportTask& operator=(NfLibraryImportTask&&) noexcept = default;
        NfLibraryImportTask(const NfLibraryImportTask&) = delete;
        NfLibraryImportTask& operator=(const NfLibraryImportTask&) = delete;
        ~NfLibraryImportTask();
        TaskStatus execute() override;
};

} // namespace NfCore

#endif // NF_THUMBNAIL_TASK_H
