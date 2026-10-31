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
#include "NfLimits.h"
#include "NfPhotoMetadata.h"
#include "NfFileUtils.h"
#include "utils/NfScopedTimer.h"

namespace NfCore {

NfRawImageDecoder::NfRawImageDecoder(const NfPhoto &photo)
        : NfImageDecoder(photo)
{
}

NfRawImageDecoder::~NfRawImageDecoder() = default;

std::optional<NfPhotoMetadata> NfRawImageDecoder::metadata() const
{
        const auto& photo = getPhoto();
        const auto& photoPath = photo.path();

        NF_LOG_DEBUG("open file: " << photoPath);
        LibRaw rawProcessor;
        if (rawProcessor.open_file(photoPath.c_str()) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't open file: " << photoPath);
                return std::nullopt;
        }

        const auto& idata = rawProcessor.imgdata.idata;
        const auto& sizes = rawProcessor.imgdata.sizes;
        const auto& other = rawProcessor.imgdata.other;
        const auto& color = rawProcessor.imgdata.color;
        const auto& lens = rawProcessor.imgdata.lens;

        NfPhotoMetadata metadata;
        metadata.width = sizes.width;
        metadata.height = sizes.height;
        if (sizes.raw_aspect >= LIBRAW_IMAGE_ASPECT_MINIMAL_REAL_ASPECT_VALUE
            && sizes.raw_aspect <= LIBRAW_IMAGE_ASPECT_MAXIMAL_REAL_ASPECT_VALUE)
                metadata.cameraAspectRatio = sizes.raw_aspect / 1000.0;
        metadata.format = NfFileUtils::extension(photoPath, NfFileUtils::Case::Upper);
        if (const auto size = photo.sizeOnDisk())
                metadata.fileSize = static_cast<std::int64_t>(size);
        metadata.cameraMaker = idata.make;
        metadata.cameraModel = idata.model;
        metadata.lens = lens.makernotes.Lens;

        metadata.iso = other.iso_speed;
        metadata.aperture = other.aperture;
        metadata.shutterSpeed = other.shutter;
        metadata.focalLength = other.focal_len;
        if (other.timestamp > 0)
                metadata.dateTaken =
                        std::chrono::system_clock::from_time_t(other.timestamp);

        metadata.description = other.desc;
        metadata.author = other.artist;
        metadata.bitDepth = static_cast<int>(color.raw_bps);
        metadata.colorSpace = color.ExifColorSpace == 1
                ? "sRGB"
                : color.ExifColorSpace == 2
                ? "Adobe RGB"
                : "";

        metadata.orientation = sizes.flip;
        metadata.hasExif = true;

        const auto& gps = other.parsed_gps;
        if (gps.gpsparsed) {
                metadata.latitude = gps.latitude[0]
                        + gps.latitude[1] / 60.0
                        + gps.latitude[2] / 3600.0;
                metadata.longitude = gps.longitude[0]
                        + gps.longitude[1] / 60.0
                        + gps.longitude[2] / 3600.0;
                if (gps.latref == 'S')
                        metadata.latitude = -metadata.latitude;
                if (gps.longref == 'W')
                        metadata.longitude = -metadata.longitude;
                metadata.altitude = gps.altitude;
                metadata.hasGps = true;
        }

        return metadata;
}

std::unique_ptr<NfImageData> NfRawImageDecoder::thumbnailImageData(int targetRes) const
{
        const auto& photo = getPhoto();
        const auto& path = photo.path();
        NfScopedTimer timer{"RAW thumbnailImageData", path};

        const auto size = photo.sizeOnDisk();
        if (size == 0) {
                NF_LOG_ERROR("image file is empty: " << path);
                return nullptr;
        }

        if (size > NfLimits::maxCompressedImageSize) {
                NF_LOG_ERROR("maximum compressed image size exceeded ("
                             << size << " > " << NfLimits::maxCompressedImageSize
                             << " bytes): " << path);
                return nullptr;
        }

        NF_LOG_DEBUG("open file: " << path);

        LibRaw rawProcessor;
        if (rawProcessor.open_file(path.c_str()) != LIBRAW_SUCCESS) {
                NF_LOG_DEBUG("can't open file : " << path);
                return nullptr;
        }

        NF_LOG_DEBUG("original size:"
                     << rawProcessor.imgdata.sizes.width
                     << "x" << rawProcessor.imgdata.sizes.height);

        int bestIndex = selectBestForTarget(rawProcessor.imgdata.thumbs_list, targetRes);
        if (bestIndex < 0) {
                NF_LOG_DEBUG("no thumbnail");
                return nullptr;
        }

        NF_LOG_DEBUG("unpack thumbnail at index: " << bestIndex);

        if (rawProcessor.unpack_thumb_ex(bestIndex) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't unpack thumbnail at index: " << bestIndex);
                return nullptr;
        }

        NF_LOG_DEBUG("thumbnail unpack: OK");

        auto imageData = decodeThumbnail(rawProcessor);
        if (!imageData)
                return nullptr;

        NF_LOG_DEBUG("thumbnail loaded:  " << path);
        NF_LOG_DEBUG("format: " << static_cast<int>(imageData->format()));
        NF_LOG_DEBUG("dimentions: " << imageData->width() << "x" << imageData->height());

        return imageData;
}

std::unique_ptr<NfImageData> NfRawImageDecoder::previewImageData(int targetRes) const
{
        const auto& photo = getPhoto();
        const auto& path = photo.path();
        NfScopedTimer timer{"RAW previewImageData", path};

        const auto size = photo.sizeOnDisk();
        if (size == 0) {
                NF_LOG_ERROR("image file is empty: " << path);
                return nullptr;
        }
        if (size > NfLimits::maxCompressedImageSize) {
                NF_LOG_ERROR("maximum compressed image size exceeded ("
                             << size << " > " << NfLimits::maxCompressedImageSize
                             << " bytes): " << path);
                return nullptr;
        }

        NF_LOG_DEBUG("open file: " << path);
        LibRaw rawProcessor;
        if (rawProcessor.open_file(path.c_str()) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't open file: " << path);
                return nullptr;
        }

        NF_LOG_DEBUG("original size:"
                     << rawProcessor.imgdata.sizes.width
                     << "x" << rawProcessor.imgdata.sizes.height);

        auto bestIndex = selectBestForTarget(rawProcessor.imgdata.thumbs_list, targetRes);
        if (bestIndex < 0) {
                NF_LOG_DEBUG("can't find preview");
                return nullptr;
        }

        NF_LOG_DEBUG("unpack preview at index: " << bestIndex);

        if (rawProcessor.unpack_thumb_ex(bestIndex) != LIBRAW_SUCCESS) {
                NF_LOG_ERROR("can't unpack the preview image");
                return nullptr;
        }

        NF_LOG_DEBUG("preview unpack: OK");

        auto imageData = decodeThumbnail(rawProcessor);
        if (!imageData)
                return nullptr;

        NF_LOG_DEBUG("preview loaded:  " << path);
        NF_LOG_DEBUG("format: " << static_cast<int>(imageData->format()));
        NF_LOG_DEBUG("dimentions: " << imageData->width() << "x" << imageData->height());

        return imageData;
}

std::unique_ptr<NfImageData>
NfRawImageDecoder::decodeThumbnail(LibRaw& rawProcessor) const
{
        const auto& thumbnail = rawProcessor.imgdata.thumbnail;
        const auto width = static_cast<std::size_t>(thumbnail.twidth);
        const auto height = static_cast<std::size_t>(thumbnail.theight);

        if (!isSupportedFormat(thumbnail.tformat)) {
                NF_LOG_ERROR("thumbnail format " << thumbnail.tformat
                             << " not supported");
                return nullptr;
        }

        if (!thumbnail.thumb || thumbnail.tlength == 0
            || thumbnail.twidth <= 0 || thumbnail.theight <= 0
            || width > NfLimits::maxImageDimension
            || height > NfLimits::maxImageDimension
            || width > NfLimits::maxDecodedImagePixels / height) {
                NF_LOG_ERROR("invalid RAW thumbnail buffer or dimensions");
                return nullptr;
        }

        if (thumbnail.tformat == LIBRAW_THUMBNAIL_BITMAP
            && width * height > thumbnail.tlength / 3) {
                NF_LOG_ERROR("truncated RAW bitmap thumbnail");
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>();
        imageData->setData(reinterpret_cast<const unsigned char*>(thumbnail.thumb),
                           thumbnail.tlength);
        const auto format = libRawToNfImageFormat(thumbnail.tformat);
        imageData->setFormat(format);
        imageData->setOrientation(rawProcessor.imgdata.sizes.flip);
        imageData->setWidth(thumbnail.twidth);
        imageData->setHeight(thumbnail.theight);

        if (!imageData->convertToARGB32Premultiplied()) {
                NF_LOG_ERROR("can't convert RAW thumbnail to ARGB");
                return nullptr;
        }

        return imageData;
}

std::unique_ptr<NfImageData> NfRawImageDecoder::fullImageData(bool reducedResolution) const
{
        const auto& photo = getPhoto();
        NfScopedTimer timer{"RAW fullImageData", photo.path()};

        const auto size = photo.sizeOnDisk();
        const auto& path = photo.path();
        if (size == 0) {
                NF_LOG_ERROR("image file is empty: " << path);
                return nullptr;
        }
        if (size > NfLimits::maxCompressedImageSize) {
                NF_LOG_ERROR("maximum compressed image size exceeded ("
                             << size << " > " << NfLimits::maxCompressedImageSize
                             << " bytes): " << path);
                return nullptr;
        }

        LibRaw rawProcessor;

        if (rawProcessor.open_file(path.c_str()) != LIBRAW_SUCCESS)
                return nullptr;

        return rawImageData(rawProcessor, reducedResolution);
}

std::unique_ptr<NfImageData>
NfRawImageDecoder::rawImageData(LibRaw& rawProcessor, bool halfSize) const
{
        const auto& photo = getPhoto();
        rawProcessor.imgdata.params.half_size = halfSize ? 1 : 0;
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

        if (rawProcessor.dcraw_process() != LIBRAW_SUCCESS)
                return nullptr;


        int err = 0;
        struct ProcessedImageDeleter {
                LibRaw* processor;
                void operator()(libraw_processed_image_t* image) const
                {
                        if (image)
                                processor->dcraw_clear_mem(image);
                }
        };
        std::unique_ptr<libraw_processed_image_t, ProcessedImageDeleter> processed{
                rawProcessor.dcraw_make_mem_image(&err),
                ProcessedImageDeleter{&rawProcessor}};
        if (!processed || processed->colors != 3) {
                return nullptr;
        }

        const auto width = static_cast<std::size_t>(processed->width);
        const auto height = static_cast<std::size_t>(processed->height);
        if (width == 0 || height == 0
            || width > std::vector<unsigned char>{}.max_size() / height / 4)
                return nullptr;

        auto imageData = std::make_unique<NfImageData>(
                static_cast<int>(width), static_cast<int>(height), 4);
        const auto pixelCount = width * height;

        const unsigned char* src = processed->data;
        unsigned char* dst = imageData->data();
        for (size_t i = 0; i < pixelCount; i++) {
                dst[0] = src[2]; // Blue
                dst[1] = src[1]; // Green
                dst[2] = src[0]; // Red
                dst[3] = 255;    // Alpha (Opaque)

                src += 3;
                dst += 4;
        }
        processed.reset();

        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);
        imageData->setOrientation(rawProcessor.imgdata.sizes.flip);

        NF_LOG_INFO("RAW image decoded:  " << photo.path());
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
