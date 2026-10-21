/**
 * File name: NfPhotoMetadataExtractor.cpp
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

#include "NfPhotoMetadataExtractor.h"
#include "NfImageDecoderFactory.h"
#include "NfImageDecoder.h"
#include "NfStringUtils.h"

#include <chrono>
#include <filesystem>

namespace NfCore {

NfPhotoMetadataExtractor::NfPhotoMetadataExtractor(const NfPhoto& photo)
        : m_photo{photo}
{
}

std::optional<NfPhotoSummary> NfPhotoMetadataExtractor::summaryInfo() const
{
        auto photoMetadata = metadata();
        if (!photoMetadata)
                return std::nullopt;

        NfPhotoSummary info;
        info.dateTaken = photoMetadata->dateTaken;
        info.cameraMaker = std::move(photoMetadata->cameraMaker);
        info.cameraModel = std::move(photoMetadata->cameraModel);
        info.lens = std::move(photoMetadata->lens);
        info.iso = photoMetadata->iso;
        info.aperture = photoMetadata->aperture;
        info.shutterSpeed = photoMetadata->shutterSpeed;
        info.focalLength = photoMetadata->focalLength;
        info.rating = photoMetadata->rating;
        info.hasExif = photoMetadata->hasExif;

        return info;
}

std::optional<NfPhotoMetadata> NfPhotoMetadataExtractor::metadata() const
{
        auto decoder = NfImageDecoderFactory::createDecoder(m_photo);
        if (!decoder)
                return std::nullopt;

        auto metadata = decoder->metadata();
        if (!metadata)
                return std::nullopt;

        if (metadata->cameraMaker == "--" | metadata->cameraMaker == "-") {
                metadata->cameraMaker = "";
                metadata->cameraModel = "";
                return metadata;
        }

        metadata->cameraMaker = NfStringUtils::trim(metadata->cameraMaker);
        metadata->cameraModel = NfStringUtils::trim(metadata->cameraModel);
        metadata->cameraModel = NfStringUtils::removePrefix(metadata->cameraModel,
                                                            metadata->cameraMaker);
        metadata->cameraModel = NfStringUtils::trim(metadata->cameraModel);

        return metadata;
}

} // namespace NfCore
