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
#include "NfImageDecoder.h"
#include "NfImageData.h"
#include "NfImage.h"
#include "NfPreview.h"

#include <stdexcept>
#include <iostream>

namespace NfCore {

NfPreviewTask::NfPreviewTask(const NfPhoto& photo,
                             std::unique_ptr<NfImage> imageContainer)
        : NfImageTask(photo, std::move(imageContainer))
{
}

NfPreviewTask::~NfPreviewTask() = default;

NfPreviewTask::TaskStatus NfPreviewTask::execute()
{
        NfImageDecoder decoder(getPhoto());
        std::unique_ptr<NfImageData> imageData;
        constexpr int maxTarget = 2000;
        constexpr int minTarget = 900;

        const auto method = extractionMethod();

        if (method == ExtractionMethod::Embedded
            || method == ExtractionMethod::Fastest) {
                imageData = decoder.previewImageData(minTarget);
        }

        if (!imageData && (method == ExtractionMethod::FromRaw
                           || method == ExtractionMethod::Fastest)) {
                imageData = decoder.rawImage();
        }

        if (!imageData)
                return TaskStatus::Failed;

        imageContainer()->setData(std::move(imageData));

        auto h = imageContainer()->height();
        if (h > maxTarget)
                imageContainer()->resize(maxTarget, maxTarget);

        return TaskStatus::Success;
}

std::unique_ptr<NfPreview> NfPreviewTask::takePreview()
{
        return std::make_unique<NfPreview>(getPhoto().id(), takeImage());
}

} // namespace NfCore
