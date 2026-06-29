/**
 * File name: PhotoScanner.cpp
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

#include "PhotoScanner.h"
#include "NfLogger.h"

namespace NfCore {

PhotoScanner::PhotoScanner()
        : m_scheduler{std::make_unique<NfScheduler>()}
        , m_threadPool{std::make_unique<NfThreadPool>(m_scheduler.get(), 1)}
{
}

PhotoScanner::~PhotoScanner()
{
        NF_LOG_DEBUG("called");
}

void PhotoScanner::setSource(const NfPhotoSource &source)
{
        std::unqieu_pre<NfTask> task;
        if (auto fsSource = dynamic_cast<const NfFilesystemPhotoSource*>(&source)) {
                task = std::make_unique<NfFilesystemPhotoScanTask>(fsSource->path());
                task->setPhotoFound([this](const NfPhoto& photo) {
                        std::scoped_lock lock(m_mutex);
                        m_loadedPhotos.push_back(std::move(photo));
                });
        }
        else if (auto dbSource = dynamic_cast<const NfLibraryPhotoSource*>(&source))
                task = std::make_unique<NfLibraryPhotoScanTask>(m_library, dbSource->query());

        if (task)
                m_scheduler->submit(std::move(task));
}

std::vector<NfPhoto> PhotoScanner::takePhotos()
{
        std::lock_guard lock(m_mutex);
        return std::move(m_loadedPhotos);
}

} // namespace NfCore
