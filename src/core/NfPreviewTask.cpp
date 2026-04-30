/**
 * File name: NfPreviewTask.cpp
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

#include "NfPreviewTask.h"
#include "NfImageDecoderFactory.h"
#include "NfImageDecoder.h"
#include "NfImageData.h"
#include "NfImage.h"
#include "NfPreview.h"
#include "NfLogger.h"

#include <stdexcept>
#include <iostream>

namespace NfCore {

NfPreviewTask::NfPreviewTask(const NfPhoto& photo)
        : NfImageTask(photo)
{
}

NfPreviewTask::~NfPreviewTask() = default;

NfPreviewTask::TaskStatus NfPreviewTask::execute()
{
        auto photo = getPhoto();
        auto decoder = NfImageDecoderFactory::createDecoder(photo);
        if (!decoder) {
                NF_LOG_ERROR("can't create decoder for photo: " << photo.path());
                return TaskStatus::Failed;
        }

        std::unique_ptr<NfImageData> imageData;
        constexpr int minTarget = 900;
        constexpr int maxTarget = 2000;

        const auto method = extractionMethod();

        if (method == ExtractionMethod::Embedded
            || method == ExtractionMethod::Fastest) {
                NF_LOG_DEBUG("load embedded image");
                imageData = decoder->thumbnailImageData(minTarget);
        }

        if (!imageData && (method == ExtractionMethod::FromRaw
                           || method == ExtractionMethod::Fastest)) {
                NF_LOG_DEBUG("no suitable embedded image, load from raw");
                imageData = decoder->fullImage();
        }

        if (!imageData) {
                NF_LOG_ERROR("can't extract image data for photo: " << photo.path());
                return TaskStatus::Failed;
        }

        auto* image = getImage();
        image->setData(std::move(imageData));
        if (image->height() > maxTarget) {
                NF_LOG_DEBUG("resize to target: " << maxTarget);
                getImage()->scaleToHeight(maxTarget);
        }

        if (image->orientation() != NfImage::Orientation::Normal) {
                NF_LOG_DEBUG("fix orientation");
                getImage()->applyOrientation();
        }

        return TaskStatus::Success;
}

std::unique_ptr<NfPreview> NfPreviewTask::takePreview()
{
        return std::make_unique<NfPreview>(getPhoto().id(), takeImage());
}

} // namespace NfCore
