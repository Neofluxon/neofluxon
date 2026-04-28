/**
 * File name: NfImageData.h
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

#ifndef NF_IMAGE_DATA_H
#define NF_IMAGE_DATA_H

#include <cstddef>
#include <vector>

namespace NfCore {

class NfImageData
{
public:

        enum class ImageFormat : int {
                Format_Unknown,
                Format_JPEG,
                Format_RGB888,
                Format_RGBA8888,
                Format_ARGB32_Premultiplied
        };

        NfImageData();
        NfImageData(size_t size);
        NfImageData(int w, int h, int c);
        ~NfImageData();
        NfImageData(const NfImageData&) = default;
        NfImageData(NfImageData&&) noexcept = default;
        NfImageData& operator=(const NfImageData&) = default;
        NfImageData& operator=(NfImageData&&) noexcept = default;
        void setData(const unsigned char* ptr, size_t len);
        const unsigned char* data() const;
        unsigned char* data();
        void resize(size_t newSize);
        size_t size() const;
        void clear();
        bool empty() const;
        void setWidth(int w);
        int width() const;
        void setHeight(int h);
        int height() const;
        void setChannels(int c);
        int channels() const;
        void setOrientation(int orientation);
        int orientation() const;
        void setFormat(ImageFormat format);
        ImageFormat format() const;
        static std::unique_ptr<NfImageData> convertedToFormat(ImageFormat format,
                                                              NfImageData *data);

private:
        std::vector<unsigned char> m_data;
        int m_width;
        int m_height;
        int m_channels;
        int m_orientation;
        ImageFormat m_format;
};

} // namespace NfCore

#endif // NF_IMAGE_DATA_H
