/**
 * File name: NfRawImageDecoder.h
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

#ifndef NF_RAW_IMAGE_DECODER_H
#define NF_RAW_IMAGE_DECODER_H

#include "NfImageDecoder.h"
#include "NfImageData.h"

#include <libraw/libraw.h>

#include <memory>

namespace NfCore {

class NfPhoto

class NfRawImageDecoder {
 public:
        explicit NfRawImageDecoder(const NfPhoto &photo);
        ~NfRawImageDecoder();
        std::unique_ptr<NfImageData> thumbnailImageData(int targetRes) const override;
        std::unique_ptr<NfImageData> previewImageData(int targetRes) const override;
        std::unique_ptr<NfImageData> fullImageData() const override;

private:
        NfImageData::ImageFormat libRawToNfImageFormat(int format);
        bool isSupportedFormat(int format)
        int selectBestForTarget(const libraw_thumbnail_list_t& list,
                                int targetSize);
};

} // namespace NfCore

#endif // NF_RAW_IMAGE_DECODER_H
