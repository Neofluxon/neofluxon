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

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

namespace NfCore {

NfImage::NfImage()
        : m_data(std::make_unique<NfImageData>())
{
}

NfImage::NfImage(std::unique_ptr<NfImageData> data)
        : m_data{std::move(data)}
{
}

NfImage::~NfImage() = default;

NfImage::NfImage(const NfImage& other)
{
        if (other.m_data)
                m_data = std::make_unique<NfImageData>(*other.m_data);
}

// Copy Assignment
NfImage& NfImage::operator=(const NfImage& other)
{
        if (this != &other) {
                if (other.m_data)
                        m_data = std::make_unique<NfImageData>(*other.m_data);
                else
                        m_data.reset();
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
        return m_data->format();
}

bool NfImage::isValid() const
{
        return !m_data->empty();
}

size_t NfImage::size() const
{
        return m_data->size();
}

NfImage::Orientation NfImage::orientation() const
{
        return static_cast<Orientation>(m_data->orientation());
}

void NfImage::applyOrientation()
{
        if (!m_data)
                return;

        if (orientation() == Orientation::Normal)
                return;

        int w = m_data->width();
        int h = m_data->height();

        const uint32_t* src = reinterpret_cast<const uint32_t*>(m_data->data());

        std::vector<unsigned char> newData;
        newData.resize(m_data->size());
        uint32_t* dst = reinterpret_cast<uint32_t*>(newData.data());

        int newW = w;
        int newH = h;

        switch (orientation()) {
        case Orientation::Rotate180: // 180° Rotation
                for (int i = 0; i < w * h; ++i)
                        dst[w * h - 1 - i] = src[i];
                break;

        case Orientation::Rotate90CW: // 90° CW (Swaps Width and Height)
                newW = h;
                newH = w;
                for (int y = 0; y < h; ++y) {
                        for (int x = 0; x < w; ++x) {
                                // Formula: newX = (h - 1 - y), newY = x
                                dst[x * h + (h - 1 - y)] = src[y * w + x];
                        }
                }
                break;

        case Orientation::Rotate270CW: // 90° CCW (Swaps Width and Height)
                newW = h;
                newH = w;
                for (int y = 0; y < h; ++y) {
                        for (int x = 0; x < w; ++x) {
                                // Formula: newX = y, newY = (w - 1 - x)
                                dst[(w - 1 - x) * h + y] = src[y * w + x];
                        }
                }
                break;

        default:
                // Standard orientaiton or unknown orientation
                return;
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
