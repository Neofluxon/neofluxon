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

#include <chrono>
#include <filesystem>

namespace NfCore {

NfPhotoMetadataExtractor::NfPhotoMetadataExtractor(const NfPhoto& photo)
        : m_photo{photo}
{
}

NfPhotoSummary NfPhotoMetadataExtractor::indexInfo() const
{
        NfPhotoSummary info;

        try {
                auto ftime = std::filesystem::last_write_time(m_photo.path());
                info.dateTaken = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
        }
        catch (...) {
        }

        // Later optimization:
        // - quick EXIF read (JPEG)
        // - LibRaw quick metadata (RAW)

        return info;
}

// ---------------- FULL METADATA ----------------

//NfPhotoMetadata NfPhotoMetadataExtractor::fullMetadata() const
//{
//        NfPhotoMetadata meta;

//        switch (m_photo.format()) {
//        case NfPhoto::PhotoFormat::Jpeg:
//        case NfPhoto::PhotoFormat::Png:
//        {
            // EXIF-based extraction (e.g. Exiv2)
            /*
            read EXIF:
            - camera maker
            - model
            - ISO
            - aperture
            - shutter
            - focal length
            - DateTimeOriginal
            */
        //            break;
        //        }
        //        case NfPhoto::PhotoFormat::Raw:
        //        {
            // RAW extraction (e.g. LibRaw)
            /*
            LibRaw raw;
            raw.open_file(m_photo.path().c_str());

            extract:
            - camera info
            - exposure settings
            - sensor metadata
            - timestamps
            */
        //            break;
        //        }
        //        default:
        //            break;
        //        }

        //        return meta;
        //}

} // namespace NfCore

