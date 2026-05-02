/**
 * File name: NfBrowserModel.cpp
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

#include "NfBrowserModel.h"
#include "NfContext.h"
#include "NfUiState.h"
#include "NfUiFolderModeState.h"
#include "NfPhotoProvider.h"
#include "core/NfLogger.h"

using namespace NfUi;

namespace NfDesktop {

NfBrowserModel::NfBrowserModel(NfContext *ctx, QObject* parent)
        : QAbstractListModel(parent)
        , m_context{ctx}
        , m_photoProvider{new NfPhotoProvider(m_context->core(), this)}
        , m_previewRange{10}
{
        QObject::connect(m_photoProvider,
                         &NfPhotoProvider::photosLoaded,
                         this,
                         &NfBrowserModel::onPhotosLoaded);
        QObject::connect(m_photoProvider,
                         &NfPhotoProvider::thumbnailsLoaded,
                         this,
                         &NfBrowserModel::onThumbnailsLoaded);
        QObject::connect(m_photoProvider,
                         &NfPhotoProvider::previewsLoaded,
                         this,
                         &NfBrowserModel::onPreviewsLoaded);
        QObject::connect(m_context->uiState->folderModeState(),
                         &NfUiFolderModeState::pathChanged,
                         this,
                         &NfBrowserModel::setPath);
 }

void NfBrowserModel::setPath(const std::filesystem::path &path)
{
        beginResetModel();
        m_photos.clear();
        m_itemsMap.clear();
        endResetModel();

        m_photoProvider->setPath(path);
}

const std::filesystem::path& NfBrowserModel::getPath() const
{
        return m_photoProvider->getPath();
}

int NfBrowserModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_photos.size());
}

QVariant NfBrowserModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() >= m_photos.size())
                return QVariant();

        switch (role) {
        case ImageDataRole::ThumbnailRole:
                return m_photoProvider->getThumbnail(m_photos[index.row()]);
        case ImageDataRole::PreviewRole:
                return getPreview(index);
        case Qt::DisplayRole:
                return QString("Photo %1").arg(index.row() + 1);
        default:
                return QVariant();
        }
}

QPixmap NfBrowserModel::getPreview(const QModelIndex& index) const
{
        if (!index.isValid() || index.row() >= m_photos.size())
                return QPixmap();

        auto centerRow = index.row();
        const auto& previewImage = m_photoProvider->getPreview(m_photos[centerRow]);

        int start = std::max(0, centerRow - m_previewRange);
        int end   = std::min(static_cast<int>(m_photos.size()) - 1,
                             centerRow + m_previewRange);

        for (int i = start; i <= end; ++i) {
                if (i == centerRow)
                        continue;
                // TODO: request only preview
                m_photoProvider->getPreview(m_photos[i]);
        }

        return previewImage;
}

void NfBrowserModel::onPhotosLoaded(const std::vector<NfPhoto> &newPhotos)
{
        const int firstRow = static_cast<int>(m_photos.size());
        const int count    = static_cast<int>(newPhotos.size());
        const int lastRow  = firstRow + count - 1;

        beginInsertRows(QModelIndex(), firstRow, lastRow);

        m_photos.reserve(m_photos.size() + count);
        m_photos.insert(m_photos.end(),
                        std::make_move_iterator(newPhotos.begin()),
                        std::make_move_iterator(newPhotos.end()));

        for (int i = 0; i < count; ++i) {
                const auto& photo = m_photos[firstRow + i];
                m_itemsMap[photo.id()] = QPersistentModelIndex(index(firstRow + i));
        }

        endInsertRows();
}

void NfBrowserModel::onThumbnailsLoaded(const std::vector<NfPhotoId> &ids)
{
        std::ranges::for_each(ids, [this](auto& id) {
                if (auto it = m_itemsMap.find(id); it != m_itemsMap.end()) {
                        const auto& idx = it->second;
                        if (idx.isValid()/* && isIndexVisible(idx)*/)
                                emit dataChanged(idx, idx, {ImageDataRole::ThumbnailRole});
                }
        });
}

void NfBrowserModel::onPreviewsLoaded(const std::vector<NfPhotoId> &ids)
{
        std::ranges::for_each(ids, [this](auto& id) {
                if (auto it = m_itemsMap.find(id); it != m_itemsMap.end()) {
                        const auto& idx = it->second;
                        if (idx.isValid()/* && isIndexVisible(idx)*/)
                                emit dataChanged(idx, idx, {ImageDataRole::PreviewRole});
                }
        });
}

void NfBrowserModel::prefetchPage(int firstVisible, int pageSize)
{
        NF_LOG_DEBUG("firstVisible: " << firstVisible << ", pageSize: " << pageSize);

        pageSize *= 2;
        int start = firstVisible + pageSize;
        int end = start + pageSize;

        end = std::min(end, static_cast<int>(m_photos.size()));

        NF_LOG_DEBUG("prefetch start: " << start << ", end: " << end);

        for (int row = start; row < end; ++row)
                m_photoProvider->prefetchThumbnail(m_photos[row]);
}


} // namespace NfDesktop
