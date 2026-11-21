/**
 * File name: NfThumbnailDelegate.h
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

#ifndef NF_THUMBNAIL_DELEGATE_H
#define NF_THUMBNAIL_DELEGATE_H

#include "NfBrowserModel.h"

#include <QIcon>
#include <QStyledItemDelegate>

namespace NfDesktop {

class NfThumbnailDelegate : public QStyledItemDelegate {
        Q_OBJECT
public:
        explicit NfThumbnailDelegate(QObject* parent = nullptr);
        void paint(QPainter* painter,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem& option,
                       const QModelIndex& index) const override;
        bool editorEvent(QEvent *event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) override;
        QRect favoriteIconRect(const QRect& itemRect) const;
        QRect favoriteHitRect(const QRect& itemRect) const;
        void setFavoriteHoverIndex(const QModelIndex& index);

signals:
        void favoriteToggleRequested(NfCore::NfPhotoId photoId, bool favorite);

private:
        QRect thumbnailRect(const QRect& itemRect) const;
        QRect controlsRect(const QRect& itemRect) const;

        QIcon m_favoriteIcon;
        QIcon m_favoriteOutlineIcon;
        QModelIndex m_favoriteHoverIndex;
        int m_padding;
        int m_controlsHeight;
};

} // namespace NfDesktop

#endif // NF_THUMBNAIL_DELEGATE_H
