/**
 * File name: NfJpegImageDecoder.cpp
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

#include "NfJpegImageDecoder.h"
#include "NfExifMetadataExtractor.h"
#include "NfImage.h"
#include "NfLimits.h"
#include "NfLogger.h"
#include "utils/NfScopedTimer.h"

#include <turbojpeg.h>

#include <fstream>

namespace NfCore {

NfJpegImageDecoder::NfJpegImageDecoder(const NfPhoto &photo)
        : NfImageDecoder(photo)
{
}

NfJpegImageDecoder::~NfJpegImageDecoder() = default;

std::optional<NfPhotoMetadata> NfJpegImageDecoder::metadata() const
{
        NfExifMetadataExtractor exifExtractor(getPhoto());
        return exifExtractor.metadata();
}

std::unique_ptr<NfImageData> NfJpegImageDecoder::thumbnailImageData(int targetRes) const
{
        return decodeScaled(targetRes);
}

std::unique_ptr<NfImageData> NfJpegImageDecoder::previewImageData(int targetRes) const
{
        return decodeScaled(targetRes);
}

std::unique_ptr<NfImageData>
NfJpegImageDecoder::fullImageData(bool reducedResolution) const
{
        constexpr int thumbnailTarget = 250;
        return decodeScaled(reducedResolution ? thumbnailTarget : 0);
}

std::unique_ptr<NfImageData>
NfJpegImageDecoder::decodeScaled(int targetRes) const
{
        NfScopedTimer timer{"JPEG decodeScaled", getPhoto().path()};
        const auto& path = getPhoto().path();

        const auto size = getPhoto().sizeOnDisk();
        if (!size) {
                NF_LOG_ERROR("can't get image file size: " << path);
                return nullptr;
        }

        if (*size == 0) {
                NF_LOG_ERROR("image file is empty: " << path);
                return nullptr;
        }
        if (*size > NfLimits::MaxCompressedImageSize) {
                NF_LOG_ERROR("maximum compressed image size exceeded ("
                             << *size << " > " << NfLimits::MaxCompressedImageSize
                             << " bytes): " << path);
                return nullptr;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
                NF_LOG_ERROR("can't open image file: " << path);
                return nullptr;
        }

        std::vector<unsigned char> jpegBuf(static_cast<std::size_t>(*size));
        if (!file.read(reinterpret_cast<char*>(jpegBuf.data()),
                       static_cast<std::streamsize>(*size))) {
                NF_LOG_ERROR("can't read image file: " << path);
                return nullptr;
        }

        struct TurboJpegDeleter {
                void operator()(void* handle) const
                {
                        if (handle)
                                tjDestroy(handle);
                }
        };

        std::unique_ptr<void, TurboJpegDeleter> decompressor(tjInitDecompress());
        if (!decompressor) {
                NF_LOG_ERROR("can't initialize JPEG decompressor: " << path);
                return nullptr;
        }

        int width      = 0;
        int height     = 0;
        int subsamp    = 0;
        int colorspace = 0;
        if (tjDecompressHeader3(decompressor.get(), jpegBuf.data(),
                                static_cast<unsigned long>(jpegBuf.size()),
                                &width, &height, &subsamp, &colorspace) != 0) {
                NF_LOG_ERROR("invalid JPEG header: " << path);
                return nullptr;
        }

        if (width <= 0 || height <= 0) {
                NF_LOG_ERROR("invalid image dimensions ["
                             << width << ","
                             << height << "]: " << path);
                return nullptr;
        }

        int scaledWidth = width;
        int scaledHeight = height;
        if (targetRes > 0) {
                int numScalingFactors = 0;
                const auto* factors = tjGetScalingFactors(&numScalingFactors);
                if (!factors || numScalingFactors <= 0) {
                        NF_LOG_ERROR("can't obtain JPEG scaling factors: "
                                     << path);
                        return nullptr;
                }

                std::size_t bestPixelCount = 0;
                for (int i = 0; i < numScalingFactors; ++i) {
                        const int w = TJSCALED(width, factors[i]);
                        const int h = TJSCALED(height, factors[i]);
                        if (w <= 0 || h <= 0)
                                continue;

                        const auto pixelCount = static_cast<std::size_t>(w)
                                * static_cast<std::size_t>(h);
                        if ((w >= targetRes || h >= targetRes)
                            && (bestPixelCount == 0 || pixelCount < bestPixelCount)) {
                                scaledWidth = w;
                                scaledHeight = h;
                                bestPixelCount = pixelCount;
                        }
                }
        }

        if (scaledWidth <= 0 || scaledHeight <= 0) {
                NF_LOG_ERROR("invalid scaled dimensions ["
                             << scaledWidth << "," << scaledHeight
                             << "]: " << path);
                return nullptr;
        }

        const auto w = static_cast<std::size_t>(scaledWidth);
        const auto h = static_cast<std::size_t>(scaledHeight);
        if (w > NfLimits::MaxImageDimension
            || h > NfLimits::MaxImageDimension) {
                NF_LOG_ERROR("image dimension exceeds maximum: "
                             << scaledWidth << "x" << scaledHeight
                             << " for " << path);
                return nullptr;
        }

        // Check maximum allowed width for the given height
        if (w == 0 || h == 0 || w > NfLimits::MaxDecodedImagePixels / h) {
                NF_LOG_ERROR("image dimensions exceed maximum: "
                             << scaledWidth << "x" << scaledHeight
                             << " for " << path);
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>(
                scaledWidth, scaledHeight, 4);
        if (tjDecompress2(decompressor.get(), jpegBuf.data(),
                          static_cast<unsigned long>(jpegBuf.size()),
                          imageData->data(), scaledWidth, 0, scaledHeight,
                          TJPF_BGRA, TJFLAG_FASTDCT) != 0) {
                NF_LOG_ERROR("can't decompress image: " << path);
                return nullptr;
        }

        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);
        if (const auto exifOrientation = NfExifMetadataExtractor(getPhoto()).orientation()) {
                imageData->setOrientation(static_cast<int>(
                                          NfImage::orientationFromExif(*exifOrientation)));
        }

        // JPEG has no alpha channel, so decoded pixels are fully opaque.
        imageData->convertToARGB32Premultiplied();

        return imageData;
}

} // namespace NfCore
