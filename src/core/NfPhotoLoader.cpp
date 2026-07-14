/**
 * File name: NfPhotoLoader.cpp
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

#include "NfPhotoLoader.h"
#include "NfCache.h"
#include "NfPhotoScanner.h"
#include "NfScheduler.h"
#include "NfImage.h"
#include "NfThumbnailTask.h"
#include "NfPreviewTask.h"
#include "NfLogger.h"

namespace NfCore {

NfPhotoLoader::NfPhotoLoader(NfPhotoLoaderContext ctx)
        : m_context {std::move(ctx)}
        , m_photoScanner{std::make_unique<NfPhotoScanner>(m_context.libraryManager)}
        , m_scheduler{m_context.scheduler}
        , m_thumbnailsCache{m_context.thumbnailCache}
        , m_previewsCache{m_context.previewCache}
        , m_generationId{0}
{
}

NfPhotoLoader::~NfPhotoLoader()
{
        NF_LOG_DEBUG("called");
}

void NfPhotoLoader::setSource(const NfPhotoSource &source)
{
        {
                std::scoped_lock lock(m_mutex);
                m_thumbnailsQueue.clear();
                m_generationId++;
        }

        m_scheduler->cancelAll();
        m_pendingThumbnailTasks.clear();

        m_source = source;
        m_photoScanner->setSource(m_source);
}

const NfPhotoSource& NfPhotoLoader::getSource() const
{
        return m_source;
}

void NfPhotoLoader::requestThumbnail(const NfPhoto &photo,
                                     NfPhotoLoader::RequestType requestType)
{
        std::scoped_lock lock(m_mutex);
        auto priority = requestTypeToPriority(requestType);

        auto it = m_pendingThumbnailTasks.find(photo.id());
        if (it != m_pendingThumbnailTasks.end()) {
                m_scheduler->updateTaskPriority(it->second, priority);
                return;
        }

        auto task = std::make_unique<NfThumbnailTask>(photo);
        task->setGenerationId(m_generationId);
        task->setPriority(priority);
        task->setExtractionMethod(NfImageTask::ExtractionMethod::Fastest);
        task->setSequence(m_sequence++);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* thumbnailTask = dynamic_cast<NfThumbnailTask*>(result);
                if (thumbnailTask) {
                        std::scoped_lock lock(m_mutex);

                        // Check if the thumbnail belongs to the current generation.
                        // If not, ignore it.
                        if (thumbnailTask->generationId() != m_generationId)
                                return;

                        auto thumbnail = thumbnailTask->takeThumbnail();
                        m_thumbnailsCache->add(thumbnail->id(), thumbnail->releaseImage());

                        auto request = requestTypeToPriority(RequestType::Visible);
                        if (thumbnailTask->priority() == static_cast<int>(request))
                                m_thumbnailsQueue.push_back(thumbnail->id());

                        m_pendingThumbnailTasks.erase(thumbnail->id());
                }
                });

        m_pendingThumbnailTasks.insert({photo.id(), task->taskId()});
        m_scheduler->submit(std::move(task));
}

void NfPhotoLoader::requestPreview(const NfPhoto &photo,
                                   NfPhotoLoader::RequestType requestType)
{
        std::scoped_lock lock(m_mutex);
        auto task = std::make_unique<NfPreviewTask>(photo);
        task->setGenerationId(m_generationId);
        task->setPriority(requestTypeToPriority(requestType));
        task->setExtractionMethod(NfImageTask::ExtractionMethod::Fastest);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* previewTask = dynamic_cast<NfPreviewTask*>(result);
                if (previewTask) {
                        std::scoped_lock lock(m_mutex);

                        // Check if the preview belongs to the current generation.
                        // If not, ignore it.
                        if (previewTask->generationId() != m_generationId)
                                return;

                        auto preview = previewTask->takePreview();
                        m_previewsCache->add(preview->id(), preview->releaseImage());

                        NF_LOG_DEBUG("push preview in the queue: " << preview->id().value());

                        m_previewsQueue.push_back(preview->id());
                }
        });

        m_scheduler->submit(std::move(task));
}

std::vector<NfPhoto> NfPhotoLoader::takePhotos()
{
        return m_photoScanner->takePhotos();
}

std::vector<NfPhotoId> NfPhotoLoader::takeThumbnails()
{
        std::scoped_lock lock(m_mutex);
        return std::move(m_thumbnailsQueue);
}

std::vector<NfPhotoId> NfPhotoLoader::takePreviews()
{
        std::scoped_lock lock(m_mutex);
        return std::move(m_previewsQueue);
}

NfTask::Priority NfPhotoLoader::requestTypeToPriority(RequestType type)
{
        switch (type) {
        case RequestType::Visible:
                return NfTask::Priority::Immediate;
        case RequestType::Prefetch:
                return NfTask::Priority::High;
        default:
                return NfTask::Priority::Normal;
        }
}

} // namespace NfCore
