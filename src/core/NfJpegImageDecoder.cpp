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
#include "NfLimits.h"
#include "NfLogger.h"

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

std::unique_ptr<NfImageData> NfJpegImageDecoder::fullImageData() const
{
        return decodeScaled(0);
}

std::unique_ptr<NfImageData>
NfJpegImageDecoder::decodeScaled(int targetRes) const
{
        const auto path = getPhoto().path().string();
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
                NF_LOG_ERROR("can't open image: " << path);
                return nullptr;
        }

        const auto end = file.tellg();
        if (end <= 0) {
                NF_LOG_ERROR("invalid or empty image file: " << path);
                return nullptr;
        }

        const auto size = static_cast<std::uintmax_t>(end);
        if (size > std::vector<unsigned char>{}.max_size()
            || size > static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max())
            || size > static_cast<std::uintmax_t>(std::numeric_limits<unsigned long>::max())) {
                NF_LOG_ERROR("image file is too large: " << path);
                return nullptr;
        }

        file.seekg(0, std::ios::beg);
        if (!file) {
                NF_LOG_ERROR("can't seek image file: " << path);
                return nullptr;
        }

        std::vector<unsigned char> jpegBuf(static_cast<std::size_t>(size));
        if (!file.read(reinterpret_cast<char*>(jpegBuf.data()),
                       static_cast<std::streamsize>(size))) {
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

        int width = 0;
        int height = 0;
        int subsamp = 0;
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

                // Select the smallest available scaling factor that keeps
                // at least one dimension >= targetRes.
                for (int i = 0; i < numScalingFactors; ++i) {
                        const int w = TJSCALED(width, factors[i]);
                        const int h = TJSCALED(height, factors[i]);
                        if (w >= targetRes || h >= targetRes) {
                                scaledWidth = w;
                                scaledHeight = h;
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

        const std::size_t bufferSize = w * h * 4;
        std::vector<unsigned char> dstBuf(bufferSize);
        if (tjDecompress2(decompressor.get(), jpegBuf.data(),
                          static_cast<unsigned long>(jpegBuf.size()),
                          dstBuf.data(), scaledWidth, 0, scaledHeight,
                          TJPF_BGRA, TJFLAG_FASTDCT) != 0) {
                NF_LOG_ERROR("can't decompress image: " << path);
                return nullptr;
        }

        auto imageData = std::make_unique<NfImageData>();
        imageData->setData(dstBuf.data(), dstBuf.size());
        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);
        imageData->setWidth(scaledWidth);
        imageData->setHeight(scaledHeight);

        // JPEG has no alpha channel, so decoded pixels are fully opaque.
        imageData->convertToARGB32Premultiplied();

        return imageData;
}

} // namespace NfCore
