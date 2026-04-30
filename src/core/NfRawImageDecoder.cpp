/**
 * File name: NfRawImageDecoder.cpp
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

#include "NfRawImageDecoder.h"
#include "NfLogger.h"

#include <omp.h>

namespace NfCore {

NfRawImageDecoder::NfRawImageDecoder(const NfPhoto &photo)
        : NfImageDecoder(photo)
{
}

NfRawImageDecoder::~NfRawImageDecoder() = default;

std::unique_ptr<NfImageData> NfRawImageDecoder::thumbnailImageData(int targetRes) const
{
        NF_LOG_DEBUG("open file: " << m_photo.path());

        auto rawProcessor = std::make_unique<LibRaw>();
        if (rawProcessor->open_file(m_photo.path().string().c_str()) != LIBRAW_SUCCESS) {
                NF_LOG_DEBUG("can't open file : " << m_photo.path());
                return nullptr;
        }

        NF_LOG_DEBUG("open file OK");
        NF_LOG_DEBUG("original size:"
                     << rawProcessor->imgdata.sizes.width
                     << "x" << rawProcessor->imgdata.sizes.height);

        int bestIndex = selectBestForTarget(rawProcessor->imgdata.thumbs_list, targetRes);
        if (bestIndex < 0) {
                NF_LOG_DEBUG("no thumbnail");
                return nullptr;
        }

        NF_LOG_DEBUG("unpack thumbnail at index: " << bestIndex);

        if (rawProcessor->unpack_thumb_ex(bestIndex) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't unpack thumbnail at index: " << bestIndex);
                return nullptr;
        }

        NF_LOG_DEBUG("thumbnail unpack: OK");

        auto &t = rawProcessor->imgdata.thumbnail;

        NF_LOG_DEBUG("thumbnail format: " << t.tformat);

        if (!isSupportedFormat(t.tformat)) {
                NF_LOG_ERROR("format " << t.tformat << " not supported");
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>();
        imageData->setData(reinterpret_cast<const unsigned char*>(t.thumb), t.tlength);
        imageData->setFormat(libRawToNfImageFormat(t.tformat));
        imageData->setOrientation(rawProcessor->imgdata.sizes.flip);
        imageData->setWidth(t.twidth);
        imageData->setHeight(t.theight);

        imageData->convertToARGB32Premultiplied();

        NF_LOG_DEBUG("thumbnail loaded:  " << m_photo.path());
        NF_LOG_DEBUG("format: " << static_cast<int>(imageData->format()));
        NF_LOG_DEBUG("dimentions: " << imageData->width() << "x" << imageData->height());

        return imageData;
}

std::unique_ptr<NfImageData> NfRawImageDecoder::previewImageData(int targetRes) const
{
        NF_LOG_DEBUG("open file: " << m_photo.path());

        auto rawProcessor = std::make_unique<LibRaw>();
        if (rawProcessor->open_file(m_photo.path().string().c_str()) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't open file: " << m_photo.path());
                return nullptr;
        }

        NF_LOG_DEBUG("open file OK");
        NF_LOG_DEBUG("original size:"
                     << rawProcessor->imgdata.sizes.width
                     << "x" << rawProcessor->imgdata.sizes.height);

        auto bestIndex = selectBestForTarget(rawProcessor->imgdata.thumbs_list, targetRes);
        if (bestIndex < 0) {
                NF_LOG_DEBUG("can't find preview");
                return nullptr;
        }

        NF_LOG_DEBUG("unpack preview at index: " << bestIndex);

        if (rawProcessor->unpack_thumb_ex(bestIndex) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't unpack the preview image");
                return nullptr;
        }

        NF_LOG_DEBUG("preview unpack: OK");

        auto &t = rawProcessor->imgdata.thumbnail;

        NF_LOG_ERROR("format: " << t.tformat);

        if (!isSupportedFormat(t.tformat)) {
                NF_LOG_ERROR("format " << t.tformat << " not supported");
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>();
        imageData->setData(reinterpret_cast<const unsigned char*>(t.thumb), t.tlength);
        imageData->setFormat(libRawToNfImageFormat(t.tformat));
        imageData->setOrientation(rawProcessor->imgdata.sizes.flip);
        imageData->setWidth(t.twidth);
        imageData->setHeight(t.theight);

        imageData->convertToARGB32Premultiplied();

        NF_LOG_DEBUG("preview loaded:  " << m_photo.path());
        NF_LOG_DEBUG("format: " << static_cast<int>(imageData->format()));
        NF_LOG_DEBUG("dimentions: " << imageData->width() << "x" << imageData->height());

        return imageData;
}

std::unique_ptr<NfImageData> NfRawImageDecoder::fullImage() const
{
        NF_LOG_DEBUG("OMP Threads available: " << omp_get_max_threads());

        LibRaw rawProcessor;

        if (rawProcessor.open_file(m_photo.path().string().c_str()) != LIBRAW_SUCCESS)
                return nullptr;

        //rawProcessor.imgdata.params.half_size = 1;
        rawProcessor.imgdata.params.use_fuji_rotate = 0;

        if (rawProcessor.unpack() != LIBRAW_SUCCESS)
                return nullptr;

        // Use linear interpolation.
        rawProcessor.imgdata.params.user_qual = 0;
        rawProcessor.imgdata.params.output_bps = 8;

        // Use Camera White Balance
        rawProcessor.imgdata.params.use_camera_wb = 1;
        rawProcessor.imgdata.params.use_auto_wb = 0;

        // Disable heavy noise reduction or brightness loops
        rawProcessor.imgdata.params.no_auto_bright = 1;

        auto start = std::chrono::high_resolution_clock::now();
        if (rawProcessor.dcraw_process() != LIBRAW_SUCCESS)
                return nullptr;

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        NF_LOG_INFO("load image time: " << duration.count() << "us");


        int err = 0;
        auto* processed = rawProcessor.dcraw_make_mem_image(&err);
        if (!processed || processed->colors != 3) {
                if (processed)
                        rawProcessor.dcraw_clear_mem(processed);
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>();

        // Calculate RGBA size: width * height * 4
        size_t pixelCount = processed->width * processed->height;
        size_t rgbaSize = pixelCount * 4;
        std::vector<unsigned char> rgbaBuffer(rgbaSize);

        const unsigned char* src = processed->data;
        unsigned char* dst = rgbaBuffer.data();
        for (size_t i = 0; i < pixelCount; i++) {
                dst[0] = src[2]; // Blue
                dst[1] = src[1]; // Green
                dst[2] = src[0]; // Red
                dst[3] = 255;    // Alpha (Opaque)

                src += 3;
                dst += 4;
        }
        // Set the data into your NfImage object
        imageData->setData(rgbaBuffer.data(), rgbaBuffer.size());
        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);
        imageData->setWidth(processed->width);
        imageData->setHeight(processed->height);
        imageData->setOrientation(rawProcessor.imgdata.sizes.flip);

        rawProcessor.dcraw_clear_mem(processed);

        NF_LOG_INFO("thumbnail loaded:  " << m_photo.path());
        NF_LOG_INFO("format: " << static_cast<int>(imageData->format()));
        NF_LOG_INFO("dimentions: " << imageData->width() << "x" << imageData->height());

        return imageData;
}

NfImageData::ImageFormat NfRawImageDecoder::libRawToNfImageFormat(int format)
{
        switch (format) {
        case LIBRAW_THUMBNAIL_JPEG:
                return NfImageData::ImageFormat::Format_JPEG;
        case LIBRAW_THUMBNAIL_BITMAP:
                return NfImageData::ImageFormat::Format_RGB888;
        default:
                return NfImageData::ImageFormat::Format_Unknown;
        }
}

bool NfRawImageDecoder::isSupportedFormat(int format)
{
        return (format == LIBRAW_THUMBNAIL_JPEG)
                || (format == LIBRAW_THUMBNAIL_BITMAP);
}

int NfRawImageDecoder::selectBestForTarget(const libraw_thumbnail_list_t& list,
                                           int targetSize)
{
        int bestIndex = -1;
        int minDistance = INT_MAX;

        NF_LOG_DEBUG("target: " << targetSize);

        for (int i = 0; i < list.thumbcount; i++) {
                int w = list.thumblist[i].twidth;
                int h = list.thumblist[i].theight;

                NF_LOG_DEBUG("[" << i << "][" << w << "x" << h << "]");

                if (h >= targetSize) {
                        NF_LOG_DEBUG("best index:" << i);
                        return i;
                }
        }

    return -1;
}

} // namespace NfCore
