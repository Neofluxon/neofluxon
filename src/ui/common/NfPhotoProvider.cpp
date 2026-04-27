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
#include "core/NfGuiCache.h"
#include "core/NfPhotoId.h"
#include "core/NfThumbnail.h"
#include "core/NfLogger.h"
#include "core/NeofluxonCore.h"
#include "NfQPixmap.h"

#include <QTimer>
#include <QCahce>

using namespace NfCore;

namespace NfUi {

NfPhotoProvider::NfPhotoProvider(NeofluxonCore *core,
                                 QObject* parent)
        : QObject(parent)
        , m_photoLoader{core->photoLoader()}
        , m_thumbnailCache{core->thumbnailCache()}
        , m_previewCache{core->previewCache()}
        , m_thumbnailPlaceholder{":/thumb_w160.jpg"}
        , m_thumbnailPixmapCache{QCahce<uint64_t, QPixmap>(150 * 1024 * 1024)}
{
        m_thubnailPixmapCache.set
        auto timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, &NfPhotoProvider::onTimeout);
        timer->start(100);
}

NfPhotoProvider::~NfPhotoProvider()
{
        NF_LOG_DEBUG("called");
}

void NfPhotoProvider::setPath(const std::filesystem::path& path)
{
        m_path = path;
        m_photoLoader->setPath(path);
}

const std::filesystem::path& NfPhotoProvider::getPath() const
{
        return m_path;
}

const QPixmap& NfPhotoProvider::getThumbnail(const NfPhoto &photo) const
{
        const auto* pixmapImage = m_thumbnailPixmapCache.object(photo.id());
        if (pixmapImage)
                return *pixmapImage;

        const auto* cacheImage = m_thumbnailCache->get(photo.id());
        if (cacheImage) {
                auto pixmap = NfQPixmap::convertToPixmap(cacheImage);
                auto size   = NfQPixmap::estimateSizeBytes(pixmap);
                m_thumbnailPixmapCache.insert(photo.id().value(),
                                              pixmap,
                                              size);
                return *pixmapImage;
        }

        m_photoLoader->requestThumbnail(photo);

        return m_thumbnailPlaceholder;
}

const QPixmap& NfPhotoProvider::getPreview(const NfPhoto &photo) const
{
        const auto* cacheImage = m_previewCache->get(photo.id());
        if (cacheImage)
                return getPixmap(cacheImage);

        m_photoLoader->requestPreview(photo)

        return m_thumbnailPlaceholder;
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
        if (thumbnails.empty())
                return;

        std::vector<NfPhotoId> photoIds;
        photoIds.reserve(thumbnails.size());

        for(auto &thumb : thumbnails) {
                m_thumbnailCache->add(thumb.id(), thumb.releaseImage());
                photoIds.push_back(thumb.id());
        }

        if (!photoIds.empty())
                emit thumbnailsLoaded(photoIds);
}

void NfPhotoProvider::processPreviews()
{
        auto previews = m_photoLoader->takePreviews();
        if (previews.empty())
                return;

        std::vector<NfPhotoId> photoIds;
        photoIds.reserve(previews.size());

        for(auto &preview : previews) {
                m_previewCache->add(preview.id(), preview.releaseImage());
                photoIds.push_back(preview.id());
        }

        if (!photoIds.empty())
                emit previewsLoaded(photoIds);
}

} // namespace NfUi
