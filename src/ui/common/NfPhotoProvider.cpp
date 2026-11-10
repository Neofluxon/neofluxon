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

#include <algorithm>
#include <limits>

using namespace NfCore;

namespace NfUi {

namespace {

int pixmapCacheCostKiB(std::uint64_t bytes)
{
        constexpr std::uint64_t bytesPerKiB = 1024;
        const auto cost = bytes / bytesPerKiB
                          + (bytes % bytesPerKiB == 0 ? 0 : 1);
        return static_cast<int>(std::min(
                cost, static_cast<std::uint64_t>(std::numeric_limits<int>::max())));
}

} // namespace

NfPhotoProvider::NfPhotoProvider(NeofluxonCore *core,
                                 QObject* parent)
        : QObject(parent)
        , m_photoLoader{core->photoLoader()}
        , m_thumbnailCache{core->thumbnailCache()}
        , m_previewCache{core->previewCache()}
        , m_thumbnailPixmapCache{QCache<uint64_t, QPixmap>(
                  pixmapCacheCostKiB(core->settings().thumbnailMemoryAllocation().pixmapBytes))}
        , m_previewPixmapCache{QCache<uint64_t, QPixmap>(
                  pixmapCacheCostKiB(core->settings().previewMemoryAllocation().pixmapBytes))}
        , m_thumbnailPlaceholder{":/placeholder_thumbnail.svg"}
        , m_previewPlaceholder{":/placeholder_preview.svg"}
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

QPixmap NfPhotoProvider::getThumbnail(const NfPhoto &photo,
                                      NfRequestType request) const
{
        auto* pixmapImage = m_thumbnailPixmapCache.object(photo.id().value());
        if (pixmapImage)
                return *pixmapImage;

        auto cacheImage = m_thumbnailCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage.get());
                cacheImage.reset();

                const auto size = NfQPixmap::estimateSizeBytes(pixmap.get());
                pixmapImage = pixmap.release();

                // Pixmap cache is called only from the GUI thread
                m_thumbnailPixmapCache.insert(photo.id().value(),
                                              pixmapImage,
                                              pixmapCacheCostKiB(size));

                return *pixmapImage;
        }

        m_photoLoader->requestThumbnail(photo, request);

        return m_thumbnailPlaceholder;
}

void NfPhotoProvider::prefetchThumbnail(const NfPhoto &photo)
{
        getThumbnail(photo, NfRequestType::PrefetchThumbnail);
}

QPixmap NfPhotoProvider::getPreview(const NfPhoto &photo,
                                    NfRequestType request) const
{
        auto* pixmapImage = m_previewPixmapCache.object(photo.id().value());
        if (pixmapImage)
                return *pixmapImage;

        auto cacheImage = m_previewCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage.get());
                cacheImage.reset();

                const auto size = NfQPixmap::estimateSizeBytes(pixmap.get());
                pixmapImage = pixmap.release();

                // Pixmap cache is called only from the GUI thread
                m_previewPixmapCache.insert(photo.id().value(),
                                              pixmapImage,
                                              pixmapCacheCostKiB(size));

                return *pixmapImage;
        }

        m_photoLoader->requestPreview(photo, request);

        return m_previewPlaceholder;
}

void NfPhotoProvider::prefetchPreview(const NfPhoto &photo)
{
        getPreview(photo, NfRequestType::PrefetchPreview);
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
        if (!previews.empty()) {
                NF_LOG_DEBUG("emit previewsLoaded: " << previews.size());
                emit previewsLoaded(previews);
        }
}

} // namespace NfUi
