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
        return fullImageData();
}

std::unique_ptr<NfImageData> NfPngImageDecoder::previewImageData(int targetRes) const
{
        return thumbnailImageData(targetRes);
}

std::unique_ptr<NfImageData> NfPngImageDecoder::fullImageData() const
{
        auto path = getPhoto().path().string();

        FILE *fp = fopen(path.c_str(), "rb");
        if (!fp)
                return nullptr;

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);

        if (!png_ptr) {
                fclose(fp);
                return nullptr;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);

        if (!info_ptr) {
                png_destroy_read_struct(&png_ptr, nullptr, nullptr);
                fclose(fp);
                return nullptr;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                fclose(fp);
                return nullptr;
        }

        png_init_io(png_ptr, fp);
        png_read_info(png_ptr, info_ptr);

        png_uint_32 width = 0;
        png_uint_32 height = 0;

        int bit_depth = 0;
        int color_type = 0;

        png_get_IHDR(png_ptr,
                     info_ptr,
                     &width,
                     &height,
                     &bit_depth,
                     &color_type,
                     nullptr,
                     nullptr,
                     nullptr);

        if (width == 0 || height == 0) {
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                fclose(fp);
                return nullptr;
        }

        // Palette -> RGB
        if (color_type == PNG_COLOR_TYPE_PALETTE)
                png_set_palette_to_rgb(png_ptr);

        // Gray < 8 bit -> 8 bit
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
                png_set_expand_gray_1_2_4_to_8(png_ptr);

        // tRNS -> alpha
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
                png_set_tRNS_to_alpha(png_ptr);

        // 16-bit -> 8-bit
        if (bit_depth == 16)
                png_set_strip_16(png_ptr);

        // Gray -> RGB
        if (color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                png_set_gray_to_rgb(png_ptr);

        // Ensure RGBA
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

        png_read_update_info(png_ptr, info_ptr);

        size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

        if (height != 0 && rowBytes > SIZE_MAX / height) {
                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
                fclose(fp);
                return nullptr;
        }

        size_t imageSize = rowBytes * static_cast<size_t>(height);

        std::vector<unsigned char> data(imageSize);

        std::vector<png_bytep> row_pointers(height);

        for (png_uint_32 i = 0; i < height; ++i)
                row_pointers[i] = data.data() + (i * rowBytes);

        png_read_image(png_ptr, row_pointers.data());
        png_read_end(png_ptr, nullptr);

        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);

        auto imageData = std::make_unique<NfImageData>();
        imageData->setData(data.data(), data.size());
        imageData->setWidth(static_cast<int>(width));
        imageData->setHeight(static_cast<int>(height));
        imageData->setFormat(NfImageData::ImageFormat::Format_RGBA8888);

        imageData->convertToARGB32Premultiplied();

        NF_LOG_DEBUG("PNG IMAGE loaded: ["
                     << imageData->width()
                     << "x"
                     << imageData->height()
                     << "]");

        return imageData;
}

} // namespace NfCore
