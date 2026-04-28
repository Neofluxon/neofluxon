/**
 * File name: NfImageData.cpp
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

#include "NfImageData.h"

#include <turbojpeg.h>
#include <cstring>

namespace NfCore {

NfImageData::NfImageData()
        : m_width{0}
        , m_height{0}
        , m_channels{0}
        , m_format{ImageFormat::Format_Unknown}
{
}

NfImageData::NfImageData(size_t size)
        : m_data(size)
        , m_width{0}
        , m_height{0}
        , m_channels{0}
        , m_format{ImageFormat::Format_Unknown}
{
}

NfImageData::NfImageData(int w, int h, int c)
        : m_data(static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(c))
        , m_width{w}
        , m_height{h}
        , m_channels{c}

{
}

NfImageData::~NfImageData()
{
}

void NfImageData::setData(const unsigned char* ptr, size_t len)
{
        m_data.assign(ptr, ptr + len);
}

const unsigned char* NfImageData::data() const
{
        return m_data.data();
}

unsigned char* NfImageData::data()
{
        return m_data.data();
}

void NfImageData::resize(size_t newSize)
{
        m_data.resize(newSize);
}

size_t NfImageData::size() const
{
        return m_data.size();
}

void NfImageData::clear()
{
        m_data.clear();
}

bool NfImageData::empty() const
{
        return m_data.empty();
}

void NfImageData::setWidth(int w)
{
        m_width = w;
}

int NfImageData::width() const
{
        return m_width;
}

void NfImageData::setHeight(int h)
{
        m_height = h;
}

int NfImageData::height() const
{
        return m_height;
}

void NfImageData::setChannels(int c)
{
        m_channels;
}

int NfImageData::channels() const
{
        return m_channels;
}

void NfImageData::setOrientation(int orientation)
{
        m_orientation = orientation;
}

int NfImageData::orientation() const
{
        return m_orientation;
}

void NfImageData::setFormat(NfImageData::ImageFormat format)
{
        m_format = format;
}

NfImageData::ImageFormat NfImageData::format() const
{
        return m_format;
}

bool NfImageData::ensureARGB32Premultiplied()
{
        if (m_format == ImageFormat::Format_ARGB32_Premultiplied)
                return true;

        // -------------------------
        // 1. JPEG decode
        // -------------------------
        if (m_format == ImageFormat::Format_JPEG) {
                tjhandle tj = tjInitDecompress();
                if (!tj)
                        return false;

                int w = 0, h = 0, subsamp = 0, cs = 0;

                if (tjDecompressHeader3(tj,
                                        m_data.data(),
                                        m_data.size(),
                                        &w, &h,
                                        &subsamp,
                                        &cs) != 0)
                        {
                                tjDestroy(tj);
                                return false;
                        }

                std::vector<unsigned char> bgra;
                bgra.resize((size_t)w * h * 4);

                if (tjDecompress2(
                                  tj,
                                  m_data.data(),
                                  m_data.size(),
                                  bgra.data(),
                                  w,
                                  0,
                                  h,
                                  TJPF_BGRA,          // A = 255
                                  TJFLAG_FASTDCT) != 0)
                        {
                                tjDestroy(tj);
                                return false;
                        }

                tjDestroy(tj);

                m_data = std::move(bgra);
                m_width = w;
                m_height = h;
                m_channels = 4;
                m_format = ImageFormat::Format_ARGB32_Premultiplied;

                return true;
        }

    // -------------------------
    // 2. RGB888 → ARGB
    // -------------------------
    if (m_format == ImageFormat::Format_RGB888)
    {
        std::vector<unsigned char> out;
        out.resize((size_t)m_width * m_height * 4);

        const unsigned char* src = m_data.data();
        unsigned char* dst = out.data();

        for (int i = 0; i < m_width * m_height; ++i)
        {
            unsigned char r = src[i * 3 + 0];
            unsigned char g = src[i * 3 + 1];
            unsigned char b = src[i * 3 + 2];

            // BGRA layout (Qt friendly)
            dst[i * 4 + 0] = b;
            dst[i * 4 + 1] = g;
            dst[i * 4 + 2] = r;
            dst[i * 4 + 3] = 255;
        }

        m_data = std::move(out);
        m_channels = 4;
        m_format = ImageFormat::Format_ARGB32_Premultiplied;

        return true;
    }

    // -------------------------
    // 3. RGBA8888 → premultiply
    // -------------------------
    if (m_format == ImageFormat::Format_RGBA8888)
    {
        unsigned char* p = m_data.data();

        for (int i = 0; i < m_width * m_height; ++i)
        {
            unsigned char a = p[i * 4 + 3];

            p[i * 4 + 0] = (p[i * 4 + 0] * a) / 255;
            p[i * 4 + 1] = (p[i * 4 + 1] * a) / 255;
            p[i * 4 + 2] = (p[i * 4 + 2] * a) / 255;
        }

        m_format = ImageFormat::Format_ARGB32_Premultiplied;
        return true;
    }

    // -------------------------
    // 4. Bitmap fallback (unknown raw)
    // -------------------------
    if (m_format == ImageFormat::Format_BITMAP)
    {
        // You must define what BITMAP means in your system.
        // Usually:
        // - decode header
        // - expand to RGB or BGRA
        return false; // placeholder
    }

    return false;
}

} // namespace NfCore
