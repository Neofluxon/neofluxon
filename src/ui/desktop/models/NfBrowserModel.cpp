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
        m_visibleIndexes.clear();
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
        if (!m_sortFields.empty())
                sortPhotos();
        endResetModel();

        rebuildPersistentIndexMap();

        emit photoIndexesUpdated();
}

void NfBrowserModel::setSortFields(const NfUiBrowserState::SortFields &fields)
{
        if (m_sortFields == fields)
                return;

        m_sortFields = fields;

        if (!m_sortFields.empty()) {
                emit layoutAboutToBeChanged();
                sortPhotos();
                rebuildPersistentIndexMap();
                emit layoutChanged();
        }
}

void NfBrowserModel::setSortOrder(const NfUiBrowserState::SortOrder sortOrder)
{
        if (m_sortOrder == sortOrder)
                return;

        m_sortOrder = sortOrder;
        if (!m_sortFields.empty()) {
                emit layoutAboutToBeChanged();
                sortPhotos();
                rebuildPersistentIndexMap();
                emit layoutChanged();
        }
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
        m_visibleIndexes.clear();
        auto visiblePhotoIndexes = std::views::iota(std::size_t{0},
                                                    m_allPhotos.size())
                | std::views::filter([this](std::size_t index) {
                        return acceptsPhoto(photoByIndex(index));
                });

        std::ranges::copy(visiblePhotoIndexes,
                          std::back_inserter(m_visibleIndexes));
}

void NfBrowserModel::sortPhotos()
{
        using SortField = NfUiBrowserState::SortField;
        const bool byName = m_sortFields.contains(SortField::Name);
        const bool byDate = m_sortFields.contains(SortField::DateTaken);
        const bool bySize = m_sortFields.contains(SortField::FileSize);
        const bool ascending = m_sortOrder == NfUiBrowserState::SortOrder::Ascending;

        auto comparator = [this, byName, byDate, bySize, ascending](const auto indexA,
                                                                    const auto indexB) {
                const auto& photoA = m_allPhotos[indexA];
                const auto& photoB = m_allPhotos[indexB];
                if (byName && photoA.name() != photoB.name())
                        return ascending ? photoA.name() < photoB.name()
                                         : photoA.name() > photoB.name();
                if (bySize && photoA.sizeOnDisk() != photoB.sizeOnDisk())
                        return ascending ? photoA.sizeOnDisk() < photoB.sizeOnDisk()
                                         : photoA.sizeOnDisk() > photoB.sizeOnDisk();
                if (byDate && photoA.dateTaken() != photoB.dateTaken())
                        return ascending ? photoA.dateTaken() < photoB.dateTaken()
                                         : photoA.dateTaken() > photoB.dateTaken();

                return false;
        };

        std::ranges::sort(m_visibleIndexes,
                          comparator);
}

void NfBrowserModel::rebuildPersistentIndexMap()
{
        m_itemsMap.clear();
        m_itemsMap.reserve(m_visibleIndexes.size());
        for (int row = 0; row < static_cast<int>(m_visibleIndexes.size()); ++row) {
                const auto& id = photoByIndex(m_visibleIndexes[row]).id();
                m_itemsMap[id] = QPersistentModelIndex(index(row));
        }
}

const NfPhoto& NfBrowserModel::photoByIndex(size_t i) const
{
        return m_allPhotos[i];
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
        return parent.isValid() ? 0 : static_cast<int>(m_visibleIndexes.size());
}

QVariant NfBrowserModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() >= m_visibleIndexes.size())
                return QVariant();

        const auto visiblePhotoIndex = m_visibleIndexes[index.row()];
        switch (role) {
        case ImageDataRole::ThumbnailRole:
                return m_photoProvider->getThumbnail(photoByIndex(visiblePhotoIndex));
        case ImageDataRole::PreviewRole:
                return getPreview(index);
        case Qt::DisplayRole:
                return QString("Photo %1").arg(index.row() + 1);
        case ImageDataRole::PhotoRole:
                return QVariant::fromValue(photoByIndex(visiblePhotoIndex));
        default:
                return QVariant();
        }
}

QPixmap NfBrowserModel::getPreview(const QModelIndex& index) const
{
        if (!index.isValid() || index.row() >= m_visibleIndexes.size())
                return QPixmap();

        auto centerRow = index.row();
        const auto &photo = photoByIndex(m_visibleIndexes[centerRow]);
        const auto& previewImage = m_photoProvider->getPreview(photo);

        int start = std::max(0, centerRow - m_previewRange);
        int end   = std::min(static_cast<int>(m_visibleIndexes.size()) - 1,
                             centerRow + m_previewRange);

        for (int i = start; i <= end; ++i) {
                if (i == centerRow)
                        continue;
                m_photoProvider->prefetchPreview(photoByIndex(m_visibleIndexes[i]));
        }

        return previewImage;
}

void NfBrowserModel::onPhotosLoaded(const std::vector<NfPhoto> &newPhotos)
{
        NF_LOG_DEBUG("NEW PHOTOS: " << newPhotos.size());

        // Filter new photo indexes
        auto newVisiblePhotoIndexesView = std::views::iota(std::size_t{0}, newPhotos.size())
                | std::views::filter([this, &newPhotos](std::size_t index) {
                        return acceptsPhoto(newPhotos[index]);
                });

        // Copy the filtered indexes.
        std::vector<std::size_t> filteredPhotoIndexes;
        filteredPhotoIndexes.reserve(newPhotos.size());
        const auto firstNewPhotoIndex = m_allPhotos.size();
        std::ranges::copy(newVisiblePhotoIndexesView
                          | std::views::transform([firstNewPhotoIndex](std::size_t index) {
                                  return firstNewPhotoIndex + index;
                          }), std::back_inserter(filteredPhotoIndexes));

        if (filteredPhotoIndexes.empty())
                return;

        // Calculate the new insertion index range.
        const auto firstNewVisibleIndex = m_visibleIndexes.size();
        const auto lastNewVisibleIndex  = firstNewVisibleIndex
                                          + filteredPhotoIndexes.size() - 1;

        beginInsertRows(QModelIndex(),
                        static_cast<int>(firstNewVisibleIndex),
                        static_cast<int>(lastNewVisibleIndex));

        // Copy all new photos
        m_allPhotos.reserve(m_allPhotos.size() + newPhotos.size());
        m_allPhotos.insert(m_allPhotos.end(),
                           newPhotos.begin(),
                           newPhotos.end());

        std::ranges::copy(filteredPhotoIndexes,
                          std::back_inserter(m_visibleIndexes));
        sortPhotos();

        endInsertRows();

        rebuildPersistentIndexMap();

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
        NF_LOG_DEBUG("PREVIEWS LOADED: " << ids.size());
        std::ranges::for_each(ids, [this](auto& id) {
                if (auto it = m_itemsMap.find(id); it != m_itemsMap.end()) {
                        const auto& idx = it->second;
                        if (idx.isValid())
                                emit dataChanged(idx, idx, {ImageDataRole::PreviewRole});
                }
        });
}

void NfBrowserModel::prefetchPage(int firstVisible,
                                  size_t pageSize,
                                  size_t prefetchPageCount)
{
        if (pageSize == 0 || m_visibleIndexes.empty())
                return;

        const auto count = m_visibleIndexes.size();
        const auto first = static_cast<std::size_t>(
              std::clamp(firstVisible, 0, static_cast<int>(count)));

        const auto start = first - std::min(first, prefetchPageCount * pageSize);

        const auto nextPage = std::min(count, first + pageSize);
        const auto end = nextPage + std::min(count - nextPage,
                                             prefetchPageCount * pageSize);

        NF_LOG_DEBUG("prefetch range: [" << start << ", " << end
                     << "), current page: [" << first << ", " << nextPage << ")");

        for (auto row = start; row < end; ++row) {
                // Exlude the current page
                if (row >= first && row < nextPage)
                        continue;

                const auto& photo = photoByIndex(m_visibleIndexes[row]);
                m_photoProvider->prefetchThumbnail(photo);
        }
}
} // namespace NfDesktop
