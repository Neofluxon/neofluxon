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

#include "NfPhotoScanner.h"
#include "NfPhotoSource.h"
#include "NfFilesystemPhotoScanTask.h"
#include "NfLibraryManager.h"
#include "NfLibraryPhotoScanTask.h"
#include "NfScheduler.h"
#include "NfThreadPool.h"
#include "NfLogger.h"

namespace NfCore {

NfPhotoScanner::NfPhotoScanner(NfLibraryManager *library)
        : m_library{library}
        , m_scheduler{std::make_unique<NfScheduler>()}
        , m_threadPool{std::make_unique<NfThreadPool>(m_scheduler.get(), 1)}
{
}

NfPhotoScanner::~NfPhotoScanner()
{
        NF_LOG_DEBUG("called");
}

std::unique_ptr<NfTask>
NfPhotoScanner::createTask(const NfFilesystemPhotoSource& source)
{
        NF_LOG_DEBUG("called");
        std::scoped_lock lock(m_mutex);
        auto task = std::make_unique<NfFilesystemPhotoScanTask>(source);
        task->setGenerationId(m_generationId);
        task->setPhotoFoundCallback([this](NfPhoto photo, uint64_t genId) {
                std::scoped_lock lock(m_mutex);
                if (genId != m_generationId)
                        return;

                m_loadedPhotos.push_back(std::move(photo));
        });

        return task;
}

std::unique_ptr<NfTask>
NfPhotoScanner::createTask(const NfLibraryPhotoSource& source)
{
        NF_LOG_DEBUG("called");
        std::scoped_lock lock(m_mutex);
        auto task = std::make_unique<NfLibraryPhotoScanTask>(source, m_library);
        task->setGenerationId(m_generationId);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                NF_LOG_DEBUG("called");

                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* libraryTask = dynamic_cast<NfLibraryPhotoScanTask*>(result);
                if (libraryTask) {
                        std::scoped_lock lock(m_mutex);

                        if (libraryTask->generationId() != m_generationId)
                                return;

                        auto photos = libraryTask->takePhotos();
                        NF_LOG_DEBUG("photos from the library: " << photos.size());
                        m_loadedPhotos.insert(m_loadedPhotos.end(),
                                              std::make_move_iterator(photos.begin()),
                                              std::make_move_iterator(photos.end()));
                }
        });

        return task;
}

void NfPhotoScanner::setSource(const NfPhotoSource& source)
{
        {
                std::scoped_lock lock(m_mutex);
                m_loadedPhotos.clear();
                ++m_generationId;
        }

        m_scheduler->cancelAll();

        auto task = std::visit([this](const auto& src) {
                return createTask(src);
        }, source);

        if (task)
                m_scheduler->submit(std::move(task));
}

std::vector<NfPhoto> NfPhotoScanner::takePhotos()
{
        std::lock_guard lock(m_mutex);
        return std::move(m_loadedPhotos);
}

} // namespace NfCore
