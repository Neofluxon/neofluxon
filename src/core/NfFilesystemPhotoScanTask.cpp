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
#include "NfImageDecoderFactory.h"
#include "NfImageDecoder.h"
#include "NfImageData.h"
#include "NfImage.h"
#include "NfLogger.h"

namespace NfCore {

NfFilesystemPhotoScanTask::NfFilesystemPhotoScanTask(const NfFilesystemPhotoSource& source)
        : m_source{source}
{
}

NfFilesystemPhotoScanTask::~NfFilesystemPhotoScanTask() = default;

NfFilesystemPhotoScanTask::TaskStatus NfFilesystemPhotoScanTask::execute()
{
        try {
                auto scan = [&](auto it, auto end) {
                        for (; it != end; ++it) {
                                if (isCanceled())
                                        return;

                                processPathEntry(it->path());
                        }
                };

                auto directory = m_source->path();
                if (recursive) {
                        NF_LOG_DEBUG("iterate dir (recursive): " << directory);

                        fs::recursive_directory_iterator it(directory), end;
                        scan(it, end);
                } else {
                        NF_LOG_DEBUG("iterate dir: " << directory);

                        fs::directory_iterator it(directory), end;
                        scan(it, end);
                }
        }
        catch (const std::filesystem::filesystem_error&) {
        }
}

void NfPathScanner::processPathEntry(const std::filesystem::path& path)
{
        namespace fs = std::filesystem;

        try {
                if (!fs::is_regular_file(path))
                        return;

                std::string ext = path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](unsigned char c) { return std::tolower(c); });

                if (!m_photoExtentions.contains(ext))
                        return;

                NfPhoto photo(path);

                NF_LOG_DEBUG("NEW PHOTO: " << photo.path());
                m_photoFoundCb(std::move(photo));
        }
        catch (const fs::filesystem_error&) {
        }
}

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

} // namespace NfCore
