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
        enum class Orientation : int {
                Normal = 0,
                FlipVertical = 1,
                FlipHorizontal = 2,
                Rotate180 = 3,
                Rotate90CWFlipVertical = 4,
                Rotate270CW = 5,
                Rotate90CW = 6,
                Rotate90CWFlipHorizontal = 7
        };

        NfImage();
        explicit NfImage(std::unique_ptr<NfImageData> data);
        virtual ~NfImage();
        NfImage(const NfImage& other);
        NfImage& operator=(const NfImage& other);
        NfImage(NfImage&& other) noexcept = default;
        NfImage& operator=(NfImage&& other) noexcept = default;
        void setData(std::unique_ptr<NfImageData> data);
        NfImageData* getData();
        const NfImageData* getData() const;
        int width() const;
        int height() const;
        int channels() const;
        ImageFormat format() const;
        bool isValid() const;
        size_t size() const;
        void scaleToHeight(int h);
        Orientation orientation() const;
        void applyOrientation();

 protected:
        std::unique_ptr<NfImageData> m_data;
};

} // namespace NfCore

#endif // NF_IMAGE_H

