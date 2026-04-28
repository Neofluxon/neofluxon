/**
 * File name: NfThumbnailTask.cpp
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

#include "NfThumbnailTask.h"
#include "NfImageDecoder.h"
#include "NfImageData.h"
#include "NfImage.h"
#include "NfLogger.h"

namespace NfCore {

NfThumbnailTask::NfThumbnailTask(const NfPhoto& photo,
                                 std::unique_ptr<NfImage> imageContainer)
        : NfImageTask(photo, std::move(imageContainer))
{
}

NfThumbnailTask::~NfThumbnailTask() = default;

NfThumbnailTask::TaskStatus NfThumbnailTask::execute()
{
        NfImageDecoder decoder(getPhoto());
        std::unique_ptr<NfImageData> imageData;
        constexpr int maxTarget = 250;
        constexpr int minTarget = 120;

        const auto method = extractionMethod();

        if (method == ExtractionMethod::Embedded
            || method == ExtractionMethod::Fastest) {
                NF_LOG_DEBUG("load embedded image");
                imageData = decoder.thumbnailImageData(minTarget);
        }

        if (!imageData && (method == ExtractionMethod::FromRaw
                           || method == ExtractionMethod::Fastest)) {
                NF_LOG_DEBUG("no suitable embedded image, load from raw");
                imageData = decoder.rawImage();
        }

        if (!imageData)
                return TaskStatus::Failed;

        imageContainer()->setData(std::move(imageData));
        if (imageContainer()->orientation() != NfImage::Orientation::Normal) {
                NF_LOG_DEBUG("fix orientation");
                imageContainer()->fixOrientation();
        }

        if (imageContainer()->height() > maxTarget) {
                NF_LOG_DEBUG("resize to target: " << maxTarget);
                imageContainer()->resizeToHeight(maxTarget, maxTarget);
        }

        return TaskStatus::Success;
}

std::unique_ptr<NfThumbnail> NfThumbnailTask::takeThumbnail()
{
        return std::make_unique<NfThumbnail>(getPhoto().id(), takeImage());
}

} // namespace NfCore
