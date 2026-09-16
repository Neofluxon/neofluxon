/**
 * File name: NfPhotoMetadataTask.cpp
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

#include "NfPhotoMetadataTask.h"
#include "NfPhotoMetadata.h"
#include "NfPhotoMetadataExtractor.h"
#include "NfLogger.h"

namespace NfCore {

NfPhotoMetadataTask::NfPhotoMetadataTask(const NfPhoto& photo)
        : m_photo{photo}
{
}

NfPhotoMetadataTask::~NfPhotoMetadataTask() = default;

NfPhotoMetadataTask::TaskStatus NfPhotoMetadataTask::execute()
{
        if (!m_photo.id().isValid()) {
                NF_LOG_ERROR("invalid photo");
                return TaskStatus::Failed;
        }

        NfPhotoMetadataExtractor extractor(m_photo);
        auto metadata = extractor.metadata();
        if (!m_metadata) {
                NF_LOG_ERROR("can't extract metadata for : " << m_photo.path());
                return TaskStatus::Failed;
        }

        m_metadata = std::make_unique<NfPhotoMetadata>(std::move(*metadata));

        return TaskStatus::Success;
}

std::unique_ptr<NfPhotoMetadata> NfPhotoMetadataTask::takeThumbnail()
{
        return std::move(m_metadata);
}

} // namespace NfCore
