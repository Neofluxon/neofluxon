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
#include "NfScheduler.h"
#include "NfLogger.h"

namespace NfCore {

NfPhotoMetadataLoader::NfPhotoMetadataLoader(NfPhotoMetadataLoaderContext ctx)
        : m_context {std::move(ctx)}
        , m_scheduler{m_context.scheduler}
{
}

NfPhotoMetadataLoader::~NfPhotoMetadataLoader()
{
        NF_LOG_DEBUG("called");
}

void NfPhotoMetadataLoader::requestMetadata(const NfPhoto &photo,
                                            NfRequestType request)
{
        std::scoped_lock lock(m_mutex);

        if (m_pendingRequests.contains(photo.id()))
                return;

        auto task = std::make_unique<NfPhotoMetadataTask>(photo);
        task->setPriority(NfRequestUtils::getTaskPriority(request));

        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                auto* metadataTask = dynamic_cast<NfPhotoMetadataTask*>(result);
                if (!metadataTask)
                        return;

                if (status != NfTask::TaskStatus::Success) {
                        std::scoped_lock lock(m_mutex);
                        m_pendingRequests.erase(metadataTask->photoId());
                        return;
                }

                std::scoped_lock lock(m_mutex);
                m_pendingRequests.erase(metadataTask->photoId());
                auto metadata = metadataTask->takeMetadata();
                if (!metadata)
                        return;

                NF_LOG_DEBUG("metadata ready for photo id: "
                             << metadataTask->photoId().value());
                m_metadataQueue.emplace_back(metadataTask->photoId(),
                                             std::move(*metadata));
        });

        m_pendingRequests.insert(photo.id());
        m_scheduler->submit(std::move(task));

        NF_LOG_DEBUG("metadata task submitted for photo: " << photo.path().string());
}

std::vector<std::pair<NfPhotoId, NfPhotoMetadata>>
NfPhotoMetadataLoader::takeMetadata()
{
        std::scoped_lock lock(m_mutex);
        return std::move(m_metadataQueue);
}

} // namespace NfCore
