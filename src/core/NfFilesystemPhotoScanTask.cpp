/**
 * File name: NfFilesystemPhotoScanTask.cpp
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

#include "NfFilesystemPhotoScanTask.h"
#include "NfLogger.h"
#include "NfFileUtils.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace NfCore {

NfFilesystemPhotoScanTask::NfFilesystemPhotoScanTask(const NfFilesystemPhotoSource& source)
        : m_source{source}
        , m_photoExtentions{".cr3", ".cr2", ".dng", ".raf", ".nef", ".jpg", ".jpeg", ".png"}
{
}

NfFilesystemPhotoScanTask::~NfFilesystemPhotoScanTask() = default;

void NfFilesystemPhotoScanTask::setRecursive(bool b)
{
        m_recursive = b;
}

bool NfFilesystemPhotoScanTask::isRecursive() const
{
        return m_recursive;
}

void NfFilesystemPhotoScanTask::setPhotoFoundCallback(PhotoFoundHandler handler)
{
        m_photoFoundCb = std::move(handler);
}

NfTask::TaskStatus NfFilesystemPhotoScanTask::execute()
{
        try {
                auto scan = [&](auto it, auto end) {
                        for (; it != end; ++it) {
                                if (isCancelled())
                                        return;

                                processPathEntry(it->path());
                        }
                };

                auto directory = m_source.path();
                if (m_recursive) {
                        NF_LOG_DEBUG("iterate dir (recursive): " << directory);

                        fs::recursive_directory_iterator it(directory), end;
                        scan(it, end);
                } else {
                        NF_LOG_DEBUG("iterate dir: " << directory);

                        fs::directory_iterator it(directory), end;
                        scan(it, end);
                }
        }
        catch (const fs::filesystem_error& error) {
                NF_LOG_ERROR("scan failed: " << error.what());
                return TaskStatus::Failed;
        }

        return TaskStatus::Success;
}

void NfFilesystemPhotoScanTask::processPathEntry(const std::filesystem::path& path)
{
        NF_LOG_DEBUG("process: " << path);

        try {
                if (!fs::is_regular_file(path))
                        return;

                auto ext = "." + NfFileUtils::extension(path,
                                                        NfFileUtils::Case::Lower);
                NF_LOG_DEBUG("extension: " << ext);

                if (ext.empty() || !m_photoExtentions.contains(ext))
                        return;

                NfPhoto photo(path);
                if (!photo.loadSummary(true)) {
                        NF_LOG_DEBUG("error on loading photo summary: " << photo.path());
                        return;
                }

                NF_LOG_DEBUG("NEW PHOTO: " << photo.path());

                m_photoFoundCb(std::move(photo), generationId());
        } catch (const fs::filesystem_error&) {
                NF_LOG_DEBUG("error processing path: " << path);
        }
}

} // namespace NfCore
