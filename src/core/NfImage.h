/**
 * File name: NfImage.h
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

#ifndef NF_IMAGE_H
#define NF_IMAGE_H

#include "NfImageData.h"

#include <memory>

namespace NfCore {

class NfImage
{
 public:
        using ImageFormat = NfImageData::ImageFormat;

        NfImage();
        explicit NfImage(std::unique_ptr<NfImageData> data);
        virtual ~NfImage();
        void setData(std::unique_ptr<NfImageData> data);
        NfImageData* getData();
        const NfImageData* getData() const;
        int width() const;
        int height() const;
        int channels() const;
        ImageFormat format() const;
        bool isValid() const;
        size_t size() const;
        void scale(int w, int h);

 protected:
        std::unique_ptr<NfImageData> m_data;
};

} // namespace NfCore

#endif // NF_IMAGE_H

