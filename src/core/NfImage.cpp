/**
 * File name: NfImage.cpp
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

#include "NfImage.h"
#include "NfLimits.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

namespace NfCore {

NfImage::NfImage()
        : m_data{std::make_unique<NfImageData>()}
{
}

NfImage::NfImage(std::unique_ptr<NfImageData> data)
        : m_data{std::move(data)}
{
}

NfImage::~NfImage() = default;

NfImage::NfImage(const NfImage& other)
            : m_data{other.m_data
                     ? std::make_unique<NfImageData>(*other.m_data)
                     : nullptr}
{
}

NfImage& NfImage::operator=(const NfImage& other)
{
        if (this != &other) {
                auto data = other.m_data
                        ? std::make_unique<NfImageData>(*other.m_data)
                        : nullptr;

                m_data = std::move(data);
        }

        return *this;
}

void NfImage::setData(std::unique_ptr<NfImageData> data)
{
        m_data = std::move(data);
}

NfImageData* NfImage::getData()
{
        return m_data.get();
}

const NfImageData* NfImage::getData() const
{
    return m_data.get();
}

int NfImage::width() const
{
    return m_data ? m_data->width() : 0;
}

int NfImage::height() const
{
    return m_data ? m_data->height() : 0;
}

int NfImage::channels() const
{
        return m_data ? m_data->channels() : 0;
}

NfImage::ImageFormat NfImage::format() const
{
        return m_data ? m_data->format() : ImageFormat::Format_Unknown;
}

bool NfImage::isValid() const
{
        return m_data && !m_data->empty();
}

size_t NfImage::size() const
{
        return m_data ? m_data->size() : 0;
}

NfImage::Orientation NfImage::orientation() const
{
        return m_data ? static_cast<Orientation>(m_data->orientation())
                      : Orientation::Normal;
}

NfImage::Orientation NfImage::orientationFromExif(int orientation)
{
        switch (orientation) {
        case 2:
                return Orientation::FlipHorizontal;
        case 3:
                return Orientation::Rotate180;
        case 4:
                return Orientation::FlipVertical;
        case 5:
                return Orientation::Rotate90CWFlipHorizontal;
        case 6:
                return Orientation::Rotate90CW;
        case 7:
                return Orientation::Rotate90CWFlipVertical;
        case 8:
                return Orientation::Rotate270CW;
        case 1:
        default:
                return Orientation::Normal;
        }
}

void NfImage::applyOrientation()
{
        if (!m_data)
                return;

        if (orientation() == Orientation::Normal)
                return;

        int w = m_data->width();
        int h = m_data->height();
        if (w <= 0 || h <= 0 || m_data->channels() != 4
            || m_data->format() != ImageFormat::Format_ARGB32_Premultiplied)
                return;

        const auto width = static_cast<std::size_t>(w);
        const auto height = static_cast<std::size_t>(h);
        if (width > NfLimits::maxDecodedImagePixels / height
            || width > m_data->size() / 4 / height
            || m_data->size() != width * height * 4)
                return;

        const uint32_t* src = reinterpret_cast<const uint32_t*>(m_data->data());

        std::vector<unsigned char> newData;
        newData.resize(m_data->size());
        uint32_t* dst = reinterpret_cast<uint32_t*>(newData.data());

        const auto imageOrientation = orientation();
        const bool swapsDimensions =
                imageOrientation == Orientation::Rotate90CWFlipVertical
                || imageOrientation == Orientation::Rotate270CW
                || imageOrientation == Orientation::Rotate90CW
                || imageOrientation == Orientation::Rotate90CWFlipHorizontal;
        const int newW = swapsDimensions ? h : w;
        const int newH = swapsDimensions ? w : h;

        for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                        int newX = x;
                        int newY = y;

                        switch (imageOrientation) {
                        case Orientation::FlipVertical:
                                newY = h - 1 - y;
                                break;
                        case Orientation::FlipHorizontal:
                                newX = w - 1 - x;
                                break;
                        case Orientation::Rotate180:
                                newX = w - 1 - x;
                                newY = h - 1 - y;
                                break;
                        case Orientation::Rotate90CWFlipVertical:
                                newX = h - 1 - y;
                                newY = w - 1 - x;
                                break;
                        case Orientation::Rotate270CW:
                                newX = y;
                                newY = w - 1 - x;
                                break;
                        case Orientation::Rotate90CW:
                                newX = h - 1 - y;
                                newY = x;
                                break;
                        case Orientation::Rotate90CWFlipHorizontal:
                                newX = y;
                                newY = x;
                                break;
                        case Orientation::Normal:
                                break;
                        }

                        dst[static_cast<std::size_t>(newY) * newW + newX]
                                = src[static_cast<std::size_t>(y) * w + x];
                }
        }

        m_data->setData(newData.data(), newData.size());
        m_data->setWidth(newW);
        m_data->setHeight(newH);
        m_data->setOrientation(static_cast<int>(Orientation::Normal));
}

void NfImage::scaleToHeight(int h)
{
        if (!m_data || m_data->height() <= 0 || h <= 0)
                return;

        float aspect = static_cast<float>(m_data->width()) / m_data->height();
        int scaledWidth = static_cast<int>(h * aspect);

        std::vector<unsigned char> scaledBuffer;
        scaledBuffer.resize(scaledWidth * h * 4);

        stbir_resize_uint8_linear(m_data->data(),
                                  m_data->width(),
                                  m_data->height(),
                                  0, // Input
                                  scaledBuffer.data(),
                                  scaledWidth,
                                  h,
                                  0,
                                  STBIR_RGBA_PM // Pixel Layout: RGBA Premultiplied
        );

        m_data->resize(scaledBuffer.size());
        m_data->setData(scaledBuffer.data(), scaledBuffer.size());
        m_data->setWidth(scaledWidth);
        m_data->setHeight(h);
}

} // namespace NfCore
