/**
 * File name: NfPhotoMetadataLoader.cpp
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

#include "NfPhotoMetadataLoader.h"
#include "NfCache.h"
#include "NfPhotoScanner.h"
#include "NfScheduler.h"
#include "NfImage.h"
#include "NfThumbnailTask.h"
#include "NfPreviewTask.h"
#include "NfLogger.h"

namespace NfCore {

NfPhotoMetadataLoader::NfPhotoMetadataLoader(NfPhotoMetadataLoaderContext ctx)
        : m_context {std::move(ctx)}
        , m_scheduler{m_context.scheduler}
        , m_thumbnailsCache{m_context.thumbnailCache}
        , m_previewsCache{m_context.previewCache}
        , m_generationId{0}
{
}

NfPhotoMetadataLoader::~NfPhotoMetadataLoader()
{
        NF_LOG_DEBUG("called");
}

void NfPhotoMetadataLoader::requestMetadata(const NfPhoto &photo, NfRequest request)
{
        std::scoped_lock lock(m_mutex);

        if (m_pendingRequests.contains(photo.id()))
                return;

        auto task = std::make_unique<NfPhotoMetadataTask>(photo);
        task->setPriority(NfRequestUtils::getTaskPriority(request));

        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                auto* metadataTask = dynamic_cast<NfMetadataTask*>(result);
                if (!metadataTask)
                        return;

                if (status != NfTask::TaskStatus::Success) {
                        std::scoped_lock lock(m_mutex);
                        m_pendingRequests.erase(metadataTask->photoId());
                        return;
                }

                std::scoped_lock lock(m_mutex);
                m_pendingRequests.erase(metadataTask->photoId());
                m_metadataQueue.insert(std::move(metadata));
        });

        m_pendingRequests.insert(photo.id());
        m_scheduler->submit(std::move(task));
}

} // namespace NfCore
