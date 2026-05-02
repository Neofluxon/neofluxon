/**
 * File name: NfPngImageDecoder.cpp
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

#include "NfPngImageDecoder.h"
#include "NfLogger.h"

#include <png.h>

namespace NfCore {

NfPngImageDecoder::NfPngImageDecoder(const NfPhoto &photo)
        : NfImageDecoder(photo)
{
}

NfPngImageDecoder::~NfPngImageDecoder() = default;

std::unique_ptr<NfImageData> NfPngImageDecoder::thumbnailImageData(int targetRes) const
{
        auto full = fullImageData();
        if (full && targetRes > 0)
                full->resize(targetRes);
        return full;
}

std::unique_ptr<NfImageData> NfPngImageDecoder::previewImageData(int targetRes) const
{
        return thumbnailImageData(targetRes);
}

std::unique_ptr<NfImageData> NfPngImageDecoder::fullImageData() const
{
        auto path = getPhoto().path().string();
        auto  *fp = fopen(path.c_str(), "rb");
        if (!fp)
                return nullptr;

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     nullptr, nullptr, nullptr);
        png_infop info_ptr = png_create_info_struct(png_ptr);

        if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                fclose(fp);
                return nullptr;
        }

        png_init_io(png_ptr, fp);
        png_read_info(png_ptr, info_ptr);

        png_uint_32 width, height;
        int bit_depth, color_type;
        png_get_IHDR(png_ptr,
                     info_ptr,
                     &width,
                     &height,
                     &bit_depth,
                     &color_type,
                     nullptr, nullptr, nullptr);

        // Convert palette to RGB
        if (color_type == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png_ptr);

        // Upscale low-bit grayscale to 8-bit
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
                png_set_expand_gray_1_2_4_to_8(png_ptr);

        // Expand transparency to alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
                png_set_tRNS_to_alpha(png_ptr);

        // Handle 16-bit files (strip down to 8-bit for standard UI)
        if (bit_depth == 16)
                png_set_strip_16(png_ptr);

        // If grayscale, convert to RGB
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                png_set_gray_to_rgb(png_ptr);

        // Force Alpha channel so we always have 4 bytes per pixel (BGRA/ARGB)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

        // Swap to BGR to match the "ARGB32" (Little Endian) byte order: B, G, R, A
        png_set_bgr(png_ptr);

        png_read_update_info(png_ptr, info_ptr);

        auto imageData = std::make_unique<NfImageData>();
        size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);
        std::vector<unsigned char> data(rowBytes * height);

        // Setup row pointers for libpng
        std::vector<png_bytep> row_pointers(height);
        for (png_uint_32 i = 0; i < height; i++)
                row_pointers[i] = data.data() + (i * rowBytes);

        // Actual Decode
        png_read_image(png_ptr, row_pointers.data());

        imageData->setData(data.data(), data.size());
        imageData->setWidth(width);
        imageData->setHeight(height);
        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);

        // Since PNGs have transparency, we MUST premultiply
        imageData->convertToARGB32Premultiplied();

        // Cleanup
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);

        return imageData;
}

} // namespace NfCore
