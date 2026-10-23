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

#include <ranges>

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
 }

void NfBrowserModel::setSource(NfPhotoSource source)
{
        beginResetModel();
        m_visiblePhotos.clear();
        m_allPhotos.clear();
        m_itemsMap.clear();
        endResetModel();

        m_photoProvider->setSource(std::move(source));
}

void NfBrowserModel::setFormatFilter(const std::set<NfPhoto::PhotoFormat>& formats)
{
        if (m_formatFilter == formats)
                return;

        m_formatFilter = formats;
        beginResetModel();
        rebuildVisiblePhotos();
        endResetModel();

        m_itemsMap.clear();
        for (int row = 0; row < static_cast<int>(m_visiblePhotos.size()); ++row)
                m_itemsMap[m_visiblePhotos[row].get().id()] = QPersistentModelIndex(index(row));

        emit photoIndexesUpdated();
}

NfPhotoSource NfBrowserModel::getSource() const
{
        return m_photoProvider->getSource();
}

bool NfBrowserModel::acceptsPhoto(const NfPhoto& photo) const
{
        return m_formatFilter.empty() || m_formatFilter.contains(photo.format());
}

void NfBrowserModel::rebuildVisiblePhotos()
{
        m_visiblePhotos.clear();
        std::ranges::copy_if(m_allPhotos, std::back_inserter(m_visiblePhotos),
                             [this](const auto& photo) {
                                     return acceptsPhoto(photo);
                             });
}

QModelIndex NfBrowserModel::indexForPhotoId(const NfPhotoId& photoId) const
{
        const auto it = m_itemsMap.find(photoId);
        if (it == m_itemsMap.end())
                return {};

        return it->second;
}

int NfBrowserModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_visiblePhotos.size());
}

QVariant NfBrowserModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() >= m_visiblePhotos.size())
                return QVariant();

        switch (role) {
        case ImageDataRole::ThumbnailRole:
                return m_photoProvider->getThumbnail(m_visiblePhotos[index.row()].get());
        case ImageDataRole::PreviewRole:
                return getPreview(index);
        case Qt::DisplayRole:
                return QString("Photo %1").arg(index.row() + 1);
        case ImageDataRole::PhotoRole:
                return QVariant::fromValue(m_visiblePhotos[index.row()].get());
        default:
                return QVariant();
        }
}

QPixmap NfBrowserModel::getPreview(const QModelIndex& index) const
{
        if (!index.isValid() || index.row() >= m_visiblePhotos.size())
                return QPixmap();

        auto centerRow = index.row();
        const auto& previewImage = m_photoProvider->getPreview(m_visiblePhotos[centerRow]);

        int start = std::max(0, centerRow - m_previewRange);
        int end   = std::min(static_cast<int>(m_visiblePhotos.size()) - 1,
                             centerRow + m_previewRange);

        for (int i = start; i <= end; ++i) {
                if (i == centerRow)
                        continue;
                m_photoProvider->prefetchPreview(m_visiblePhotos[i]);
        }

        return previewImage;
}

void NfBrowserModel::onPhotosLoaded(const std::vector<NfPhoto> &newPhotos)
{
        // Keep the index of the first new photo to add.
        const auto firstNewPhoto = m_allPhotos.size();

        m_allPhotos.reserve(m_allPhotos.size() + newPhotos.size());
        m_allPhotos.insert(m_allPhotos.end(),
                           newPhotos.begin(),
                           newPhotos.end());

        // Filter the visible photos into a temporary vector of references.
        auto newVisiblePhotos = std::vector<std::reference_wrapper<const NfPhoto>>{};
        auto visiblePhotoRefs = m_allPhotos
                | std::views::drop(firstNewPhoto)
                | std::views::filter([this](const auto& photo) {
                        return acceptsPhoto(photo);
                  })
                | std::views::transform([](const auto& photo) {
                        return std::cref(photo);
                  });

        // Copy the filtered view into a vector to allow random access.
        std::ranges::copy(visiblePhotoRefs,
                          std::back_inserter(newVisiblePhotos));

        if (newVisiblePhotos.empty())
                return;

        // Calculate the new insertion index range.
        const auto firstNewVisible = m_visiblePhotos.size();
        const auto lastNewVisible = firstNewVisible + newVisiblePhotos.size() - 1;

        beginInsertRows(QModelIndex(),
                        static_cast<int>(firstNewVisible),
                        static_cast<int>(lastNewVisible));

        // Move the new visible photo references into the visible references vector.
        m_visiblePhotos.insert(m_visiblePhotos.end(),
                               std::make_move_iterator(newVisiblePhotos.begin()),
                               std::make_move_iterator(newVisiblePhotos.end()));

        endInsertRows();

        for (auto i = firstNewVisible; i < m_visiblePhotos.size(); ++i) {
                const auto& photo = m_visiblePhotos[i].get();
                m_itemsMap[photo.id()] = QPersistentModelIndex(index(i));
        }

        emit photoIndexesUpdated();
}

void NfBrowserModel::onThumbnailsLoaded(const std::vector<NfPhotoId> &ids)
{
        NF_LOG_DEBUG("THUB LOADED: " << ids.size());
        std::ranges::for_each(ids, [this](auto& id) {
                if (auto it = m_itemsMap.find(id); it != m_itemsMap.end()) {
                        const auto& idx = it->second;
                        if (idx.isValid())
                                emit dataChanged(idx, idx, {ImageDataRole::ThumbnailRole});
                }
        });
}

void NfBrowserModel::onPreviewsLoaded(const std::vector<NfPhotoId> &ids)
{
        std::ranges::for_each(ids, [this](auto& id) {
                if (auto it = m_itemsMap.find(id); it != m_itemsMap.end()) {
                        const auto& idx = it->second;
                        if (idx.isValid())
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

        end = std::min(end, static_cast<int>(m_visiblePhotos.size()));

        NF_LOG_DEBUG("prefetch start: " << start << ", end: " << end);

        for (int row = start; row < end; ++row)
                m_photoProvider->prefetchThumbnail(m_visiblePhotos[row].get());
}
} // namespace NfDesktop
