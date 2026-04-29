/**
 * File name: NfQPixmap.cpp
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

#include "NfQPixmap.h"
#include "core/NfImage.h"
#include "core/NfImageData.h"

#include <QPixmap>
#include <QImage>

using namespace NfCore;

namespace NfUi {

std::unique_ptr<QPixmap> NfQPixmap::convertToPixmap(const NfImage *image)
{
        if (!image || !image->isValid())
                return nullptr;

        const auto* imageData = image->getData();
        const auto* rawPtr = reinterpret_cast<const uchar*>(imageData->data());

        auto pixmap = std::make_unique<QPixmap>();
        bool isLoaded = false;

        if (imageData->format() == NfImageData::ImageFormat::Format_JPEG) {
                isLoaded = pixmap->loadFromData(rawPtr, imageData->size());
        } else {
                QImage::Format qtFmt;

                switch (imageData->format()) {
                case NfImageData::ImageFormat::Format_RGB888:
                        qtFmt = QImage::Format_RGB888;
                        break;

                case NfImageData::ImageFormat::Format_RGBA8888:
                        qtFmt = QImage::Format_RGBA8888;
                        break;

                case NfImageData::ImageFormat::Format_ARGB32_Premultiplied:
                        qtFmt = QImage::Format_ARGB32_Premultiplied;
                        break;

                default:
                        return nullptr;
                }

                QImage img(rawPtr, image->width(), image->height(), qtFmt);

                *pixmap = QPixmap::fromImage(img);
                isLoaded = !pixmap->isNull();
        }

        if (!isLoaded)
                return nullptr;

        return pixmap;
}

std::size_t NfQPixmap::estimateSizeBytes(const QPixmap *pixmap)
{
        if (!pixmap || pixmap->isNull())
                return 0;

        return static_cast<std::size_t>(pixmap->width()) *
                static_cast<std::size_t>(pixmap->height()) *
                static_cast<std::size_t>(pixmap->depth()) / 8;
}

} // namespace NfUi
