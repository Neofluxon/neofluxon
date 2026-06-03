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

#ifndef NF_PHOTO_METADATA_EXTRACTOR
#define NF_PHOTO_METADATA_EXTRACTOR

#include "NfPhotoSummary.h"
#include "NfPhoto.h"
//#include "NfPhotoMetadata.h"

namespace NfCore {

class NfPhoto;

class NfPhotoMetadataExtractor {
 public:
        explicit NfPhotoMetadataExtractor(const NfPhoto& photo);
        NfPhotoSummary summaryInfo() const;
        //NfPhotoMetadata fullMetadata() const;

  private:
        NfPhoto m_photo;
};

}; // namespace NfCore

#endif // NF_PHOTO_METADATA_EXTRACTOR
