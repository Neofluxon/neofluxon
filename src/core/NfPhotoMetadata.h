/**
 * File name: NfPhotoMetadataExtractor.h
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

#ifndef NF_PHOTO_METADATA_H
#define NF_PHOTO_METADATA_H

#include <chrono>
#include <cstdint>
#include <string>

namespace NfCore {

struct NfPhotoMetadata
{
        // Image
        int width = 0;
        int height = 0;
        std::string format;
        std::int64_t fileSize = 0;

        // Capture
        std::chrono::system_clock::time_point dateTaken;
        std::string cameraMaker;
        std::string cameraModel;
        std::string lens;
        int iso = -1;
        double aperture = 0.0;
        double shutterSpeed = 0.0;
        double focalLength = 0.0;

        // Location
        double latitude = 0.0;
        double longitude = 0.0;
        double altitude = 0.0;
        bool hasGps = false;

        // Organization
        int rating = 0;
        std::string tags;

        // Description
        std::string title;
        std::string description;
        std::string author;
        std::string license;

        // Technical
        int bitDepth = 0;
        std::string colorSpace;
        int orientation = 1;

        // Availability
        bool hasExif = false;
};

} // namespace NfCore

#endif // NF_PHOTO_METADATA_H

