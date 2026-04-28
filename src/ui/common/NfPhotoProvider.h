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

#ifndef NF_PHOTO_PROVIDER_H
#define NF_PHOTO_PROVIDER_H

#include "core/NfPhoto.h"
#include "core/NfPhotoId.h"

#include <QObject>
#include <QPixmap>

#include <filesystem>
#include <vector>
#include <cstdint>

namespace NfCore {
class NeofluxonCore;
class NfPhotoLoader;
class NfCache;
}

using namespace NfCore;

Q_DECLARE_METATYPE(NfPhoto)
Q_DECLARE_METATYPE(NfPhotoId)
Q_DECLARE_METATYPE(std::vector<NfPhoto>)
Q_DECLARE_METATYPE(std::vector<NfPhotoId>)

namespace NfUi {

class NfPhotoProvider : public QObject
{
        Q_OBJECT

public:
        explicit NfPhotoProvider(NeofluxonCore *core,
                                 QObject* parent = nullptr);
        ~NfPhotoProvider();

        void setPath(const std::filesystem::path& path);
        const std::filesystem::path& getPath() const;

        const QPixmap& getThumbnail(const NfPhoto &photo) const;
        const QPixmap& getPreview(const NfPhoto &photo) const;
        void reqiestPreview(const NfPhoto &photo) const;

signals:
        void photosLoaded(const std::vector<NfPhoto>& photos);
        void thumbnailsLoaded(const std::vector<NfPhotoId>& ids);
        void previewsLoaded(const std::vector<NfPhotoId>& ids);

private slots:
        void onTimeout();

private:
        void processNewPhotos();
        void processThumbnails();
        void processPreviews();

        NfPhotoLoader *m_photoLoader;
        NfCache *m_thumbnailCache;
        NfCache *m_previewCache;
        QCahce<uint64_t, QPixmap> m_thumbnailPixmapCache;
        QCahce<uint64_t, QPixmap>m_previewPixmapCache;
        std::filesystem::path m_path;
        QPixmap m_thumbnailPlaceholder;
        QPixmap m_previewPlaceholder;
};

} // namespace NfUi

#endif // NF_PHOTO_PROVIDER_H

