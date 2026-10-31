/**
 * File name: NfBrowserModel.h
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

#ifndef NF_BROWSER_MODEL_H
#define NF_BROWSER_MODEL_H

#include "core/NfPhoto.h"
#include "core/NfPhotoId.h"
#include "core/NfPhotoSource.h"
#include "NfUiBrowserState.h"

#include <QAbstractListModel>
#include <functional>
#include <set>

Q_DECLARE_METATYPE(NfCore::NfPhoto)
Q_DECLARE_METATYPE(NfCore::NfPhotoId)
Q_DECLARE_METATYPE(std::vector<NfCore::NfPhoto>)
Q_DECLARE_METATYPE(std::vector<NfCore::NfPhotoId>)

namespace NfUi {
struct NfContext;
class NfPhotoProvider;
}

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

/**
 * NfBrowserModel provides a Qt list model for browsing photos.
 *
 * Filtering: The model uses references in m_visiblePhotos to expose the
 * filtered photos stored in m_allPhotos.
 */
class NfBrowserModel : public QAbstractListModel
{
        Q_OBJECT

public:
        enum ImageDataRole {
                ThumbnailRole = Qt::DecorationRole,
                PreviewRole = Qt::UserRole + 1,
                PhotoRole = PreviewRole + 1
        };

        explicit NfBrowserModel(NfContext *ctx,
                                QObject* parent = nullptr);
        ~NfBrowserModel() = default;
        void setSource(NfCore::NfPhotoSource source);
        NfCore::NfPhotoSource getSource() const;
        void setFormatFilter(const std::set<NfCore::NfPhoto::PhotoFormat>& formats);
        // Returns the model index for a photo ID, or an invalid index if not found.
        QModelIndex indexForPhotoId(const NfCore::NfPhotoId& photoId) const;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        void prefetchPage(int firstVisible, int pageSize);

signals:
        void modelUpdated();
        void previewReady(const QModelIndex& index);
        void photoIndexesUpdated();

public slots:
        void setSortFields(const NfUi::NfUiBrowserState::SortFields &fields);
        void setSortOrder(const NfUi::NfUiBrowserState::SortOrder sortOrder);

protected slots:
        void onPhotosLoaded(const std::vector<NfPhoto> &newPhotos);
        void onThumbnailsLoaded(const std::vector<NfPhotoId> &ids);
        void onPreviewsLoaded(const std::vector<NfPhotoId> &ids);

private:
        QPixmap getPreview(const QModelIndex& index) const;
        bool acceptsPhoto(const NfPhoto& photo) const;
        void rebuildVisiblePhotos();
        void rebuildPersistentIndexMap();
        void sortPhotos();
        const NfPhoto& photoByIndex(size_t i) const;

        NfContext* m_context;
        NfPhotoProvider *m_photoProvider;
        // Owns all loaded photos, whether or not they pass the active filter.
        // This vector is kept stable.
        std::vector<NfPhoto> m_allPhotos;
        // Indexes to filtered photos in m_allPhotos, indexed by model row.
        // The model works only with m_visibleIndexes.
        std::vector<std::size_t> m_visibleIndexes;
        std::set<NfPhoto::PhotoFormat> m_formatFilter;
        /**
         * indexForPhotoId() finds a photo's row without searching m_visiblePhotos.
         * When thumbnails or previews finish loading asynchronously, the
         * provider reports photo IDs. This map lets the model find the
         * corresponding index and emit dataChanged() for that row.
         * QPersistentModelIndex keeps indexes valid as rows are inserted or
         * moved. The map is cleared and rebuilt when the model resets or the
         * filter changes.
         */
        std::unordered_map<NfPhotoId, QPersistentModelIndex> m_itemsMap;
        const int m_previewRange;
        NfUi::NfUiBrowserState::SortFields m_sortFields;
        NfUi::NfUiBrowserState::SortOrder m_sortOrder{};
};

} // namespace NfDesktop

#endif // NF_BROWSER_MODEL_H
