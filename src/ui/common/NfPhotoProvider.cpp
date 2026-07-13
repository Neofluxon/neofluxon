/**
 * File name: NfPhotoProvider.cpp
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

#include "NfPhotoProvider.h"
#include "core/NfPhotoLoader.h"
#include "core/NfCache.h"
#include "core/NfPhotoId.h"
#include "core/NfThumbnail.h"
#include "core/NfLogger.h"
#include "core/NeofluxonCore.h"
#include "core/NfImage.h"
#include "NfQPixmap.h"

#include <QTimer>

using namespace NfCore;

namespace NfUi {

NfPhotoProvider::NfPhotoProvider(NeofluxonCore *core,
                                 QObject* parent)
        : QObject(parent)
        , m_photoLoader{core->photoLoader()}
        , m_thumbnailCache{core->thumbnailCache()}
        , m_previewCache{core->previewCache()}
        , m_thumbnailPixmapCache{QCache<uint64_t, QPixmap>(150 * 1024 * 1024)}
        , m_previewPixmapCache{QCache<uint64_t, QPixmap>(50 * 1024 * 1024)}
        , m_thumbnailPlaceholder{":/thumb_w160.jpg"}
        , m_previewPlaceholder{":/thumb_w160.jpg"}
{
        auto timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, &NfPhotoProvider::onTimeout);
        timer->start(100);
}

NfPhotoProvider::~NfPhotoProvider()
{
        NF_LOG_DEBUG("called");
}

void NfPhotoProvider::setSource(const NfPhotoSource &source)
{
        m_source = source;
        m_photoLoader->setSource(m_source);
}

const NfPhotoSource& NfPhotoProvider::getSource() const
{
        return m_source;
}

QPixmap NfPhotoProvider::getThumbnail(const NfPhoto &photo) const
{
        auto* pixmapImage = m_thumbnailPixmapCache.object(photo.id().value());
        if (pixmapImage)
                return *pixmapImage;

        auto cacheImage = m_thumbnailCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage.get());
                cacheImage.reset();

                auto size   = NfQPixmap::estimateSizeBytes(pixmap.get());
                pixmapImage = pixmap.release();

                // Pixmap cache is called only from the GUI thread
                m_thumbnailPixmapCache.insert(photo.id().value(),
                                              pixmapImage,
                                              size);

                return *pixmapImage;
        }

        m_photoLoader->requestThumbnail(photo);

        return m_thumbnailPlaceholder;
}

void NfPhotoProvider::prefetchThumbnail(const NfPhoto &photo)
{
        auto* pixmapImage = m_thumbnailPixmapCache.object(photo.id().value());
        if (pixmapImage)
                return;

        auto cacheImage = m_thumbnailCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage.get());
                cacheImage.reset();

                auto size = NfQPixmap::estimateSizeBytes(pixmap.get());
                pixmapImage = pixmap.release();

                // Pixmap cache is called only from the GUI thread
                m_thumbnailPixmapCache.insert(photo.id().value(),
                                              pixmapImage,
                                              size);
                return;
        }

        m_photoLoader->requestThumbnail(photo, NfPhotoLoader::RequestType::Prefetch);
}

QPixmap NfPhotoProvider::getPreview(const NfPhoto &photo) const
{
        auto* pixmapImage = m_previewPixmapCache.object(photo.id().value());
        if (pixmapImage)
                return *pixmapImage;

        auto cacheImage = m_previewCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage.get());
                cacheImage.reset();

                auto size   = NfQPixmap::estimateSizeBytes(pixmap.get());
                pixmapImage = pixmap.release();

                // Pixmap cache is called only from the GUI thread
                m_previewPixmapCache.insert(photo.id().value(),
                                              pixmapImage,
                                              size);

                return *pixmapImage;
        }

        m_photoLoader->requestPreview(photo);

        return m_previewPlaceholder;
}

void NfPhotoProvider::onTimeout()
{
        processNewPhotos();
        processThumbnails();
        processPreviews();
}

void NfPhotoProvider::processNewPhotos()
{
        auto newPhotos = m_photoLoader->takePhotos();
        if (!newPhotos.empty()) {
                NF_LOG_DEBUG("NEW PHOTOS: " << newPhotos.size());
                emit photosLoaded(newPhotos);
        }
}

void NfPhotoProvider::processThumbnails()
{
        auto thumbnails = m_photoLoader->takeThumbnails();
        if (!thumbnails.empty())
                emit thumbnailsLoaded(thumbnails);
}

void NfPhotoProvider::processPreviews()
{
        auto previews = m_photoLoader->takePreviews();
        if (!previews.empty())
                emit previewsLoaded(previews);
}

} // namespace NfUi
