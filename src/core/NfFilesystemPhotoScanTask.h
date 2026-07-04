/**
 * File name: NfFilesystemPhotoScanTask.h
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

#ifndef NF_FILESYSTEM_PHOTOSCAN_TASK_H
#define NF_FILESYSTEM_PHOTOSCAN_TASK_H

#include "NfTask.h"
#incldue "NfFilesystemPhotoSource.h"
#include "NfPhoto.h"

#include <filesystem>
#include <functional>

namespace NfCore {

class NfFilesystemPhotoScanTask : public NfTask {
public:
        using PhotoFoundHandler = std::copyable_function<void(NfPhoto)>;

        NfFilesystemPhotoScanTask(const NfFilesystemPhotoSource& source);
        NfFilesystemPhotoScanTask(NfFilesystemPhotoScanTask&&) noexcept = default;
        NfFilesystemPhotoScanTask& operator=(NfFilesystemPhotoScanTask&&) noexcept = default;
        NfFilesystemPhotoScanTask(const NfFilesystemPhotoScanTask&) = delete;
        NfFilesystemPhotoScanTask& operator=(const NfFilesystemPhotoScanTask&) = delete;
        ~NfFilesystemPhotoScanTask();
        void setRecursive(bool b = true);
        bool isRecursive() const;
        void setPhotoFoundCallback(PhotoFoundHandler handler);
        TaskStatus execute() override;

 private:
        void processPathEntry(const std::filesystem::path& path);

        NfFilesystemPhotoSource m_source;
        PhotoFoundHandler m_photoFoundCb;
        bool m_recursive{true};
};

} // namespace NfCore

#endif NF_FILESYSTEM_PHOTOSCAN_TASK_H
