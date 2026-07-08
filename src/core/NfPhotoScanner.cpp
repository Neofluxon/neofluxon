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
#include "NfPhotoSource.h"
#include "NfFilesystemPhotoScanTask.h"
#include "NfLibraryPhotoScanTask.h"
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

void PhotoScanner::setSource(const NfPhotoSource *source)
{
        {
                std::scoped_lock lock(m_mutex);
                m_loadedPhotos.clear();
                m_generationId++;
        }

        m_scheduler->cancelAll();

        std::unqiue_ptr<NfTask> task;
        if (auto fsSource = dynamic_cast<const NfFilesystemPhotoSource*>(&source)) {
                task = std::make_unique<NfFilesystemPhotoScanTask>(*fsSource);
                task->setGenerationId(m_generationId);
                task->setPhotoFound([this](const NfPhoto& photo) {
                        std::scoped_lock lock(m_mutex);
                        if (task->generationId() != m_generationId)
                                return;

                        m_loadedPhotos.push_back(std::move(photo));
                });
        } else if (auto librarySource = dynamic_cast<const NfLibraryPhotoSource*>(&source)) {
                task = std::make_unique<NfLibraryPhotoScanTask>(*librarySource);
                task->setGenerationId(m_generationId);
                task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                        if (status != NfTask::TaskStatus::Success)
                                return;

                        auto* libraryTask = dynamic_cast<NfLibraryPhotoScanTask*>(result);
                        if (libraryTask) {
                                std::scoped_lock lock(m_mutex);

                                if (libraryTask->generationId() != m_generationId)
                                        return;

                                m_loadedPhotos.append_range(libraryTask->takePhotos()
                                                            | std::views::as_rvalue);
                        }
        }

        if (task)
                m_scheduler->submit(std::move(task));
}

std::vector<NfPhoto> PhotoScanner::takePhotos()
{
        std::lock_guard lock(m_mutex);
        return std::move(m_loadedPhotos);
}

} // namespace NfCore
