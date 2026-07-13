/**
 * File name: NfPhotoProvider.h
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

#ifndef NF_PHOTO_PROVIDER_H
#define NF_PHOTO_PROVIDER_H

#include "core/NfPhoto.h"
#include "core/NfPhotoId.h"
#include "core/NfPhotoSource.h"

#include <QObject>
#include <QPixmap>
#include <QCache>

#include <filesystem>
#include <vector>
#include <cstdint>

namespace NfCore {
class NeofluxonCore;
class NfPhotoLoader;
class NfCache;
}

Q_DECLARE_METATYPE(NfPhoto)
Q_DECLARE_METATYPE(NfPhotoId)
Q_DECLARE_METATYPE(std::vector<NfCore::NfPhoto>)
Q_DECLARE_METATYPE(std::vector<NfCore::NfPhotoId>)

namespace NfUi {

class NfPhotoProvider : public QObject
{
        Q_OBJECT

public:
        explicit NfPhotoProvider(NfCore::NeofluxonCore *core,
                                 QObject* parent = nullptr);
        ~NfPhotoProvider();

        void setSource(const NfCore::NfPhotoSource &source);
        NfCore::NfPhotoSource getSource() const;
        QPixmap getThumbnail(const NfCore::NfPhoto &photo) const;
        QPixmap getPreview(const NfCore::NfPhoto &photo) const;
        void prefetchThumbnail(const NfCore::NfPhoto &photo);

signals:
        void photosLoaded(const std::vector<NfCore::NfPhoto>& photos);
        void thumbnailsLoaded(const std::vector<NfCore::NfPhotoId>& ids);
        void previewsLoaded(const std::vector<NfCore::NfPhotoId>& ids);

private slots:
        void onTimeout();

private:
        void processNewPhotos();
        void processThumbnails();
        void processPreviews();

        NfCore::NfPhotoLoader *m_photoLoader;
        NfCore::NfCache *m_thumbnailCache;
        NfCore::NfCache *m_previewCache;
        mutable QCache<uint64_t, QPixmap> m_thumbnailPixmapCache;
        mutable QCache<uint64_t, QPixmap>m_previewPixmapCache;
        NfCore::NfPhotoSource m_source;
        QPixmap m_thumbnailPlaceholder;
        QPixmap m_previewPlaceholder;
};

} // namespace NfUi

#endif // NF_PHOTO_PROVIDER_H
