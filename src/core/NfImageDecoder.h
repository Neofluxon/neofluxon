/**
 * File name: NfImageDecoder.h
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

#ifndef NF_IMAGE_DECODER_H
#define NF_IMAGE_DECODER_H

#include "NfPhoto.h"
#include "NfImageData.h"

#include <memory>

namespace NfCore {

class NfImageData;

class NfImageDecoder {
 public:
        explicit NfImageDecoder(const NfPhoto &photo);
        virtual ~NfImageDecoder() = default;
        virtual std::unique_ptr<NfImageData> thumbnailImageData(int targetRes) const = 0;
        virtual std::unique_ptr<NfImageData> previewImageData(int targetRes) const = 0;
        virtual std::unique_ptr<NfImageData> fullImageData() const = 0;

protected:
        const NfPhoto& getPhoto() const;

private:
        NfPhoto m_photo;
};

} // namespace NfCore

#endif // NF_IMAGE_DECODER_H
