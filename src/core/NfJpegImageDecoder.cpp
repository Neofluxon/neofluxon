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
#include "NfLogger.h"

#include <turbojpeg.h>

#include <fstream>

namespace NfCore {

NfJpegImageDecoder::NfJpegImageDecoder(const NfPhoto &photo)
        : NfImageDecoder(photo)
{
}

NfJpegImageDecoder::~NfJpegImageDecoder() = default;

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

// Private helper to handle the TurboJPEG logic
std::unique_ptr<NfImageData> NfJpegImageDecoder::decodeScaled(int targetRes) const
{
        std::string path = getPhoto().path().string();

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
                return nullptr;

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> jpegBuf(size);
        if (!file.read(reinterpret_cast<char*>(jpegBuf.data()), size))
                return nullptr;

        tjhandle decompressor = tjInitDecompress();
        int width, height, subsamp, colorspace;
        tjDecompressHeader3(decompressor,
                            jpegBuf.data(),
                            size,
                            &width,
                            &height,
                            &subsamp,
                            &colorspace);

        int scaledWidth = width;
        int scaledHeight = height;

        tjscalingfactor scalingFactor = {1, 1};
        if (targetRes > 0) {
                int numScalingFactors;
                tjscalingfactor* factors = tjGetScalingFactors(&numScalingFactors);

                // Find the smallest factor that is still >= targetRes
                for (int i = 0; i < numScalingFactors; i++) {
                        int w = TJSCALED(width, factors[i]);
                        int h = TJSCALED(height, factors[i]);
                        if (w >= targetRes || h >= targetRes) {
                                scalingFactor = factors[i];
                                scaledWidth = w;
                                scaledHeight = h;
                        }
                }
        }

        auto imageData = std::make_unique<NfImageData>();
        std::vector<unsigned char> dstBuf(scaledWidth * scaledHeight * 4);

        // Decompress directly into BGRA (which maps to ARGB on little-endian)
        int flags = TJFLAG_FASTDCT;
        if (tjDecompress2(decompressor, jpegBuf.data(), size, dstBuf.data(),
                          scaledWidth, 0, scaledHeight, TJPF_BGRA, flags) != 0) {
                tjDestroy(decompressor);
                return nullptr;
        }

        imageData->setData(dstBuf.data(), dstBuf.size());
        imageData->setFormat(NfImageData::ImageFormat::Format_ARGB32_Premultiplied);
        imageData->setWidth(scaledWidth);
        imageData->setHeight(scaledHeight);

        imageData->convertToARGB32Premultiplied();

        tjDestroy(decompressor);

        return imageData;
}

} // namespace NfCore
