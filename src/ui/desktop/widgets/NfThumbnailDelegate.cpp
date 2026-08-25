/**
 * File name: NfThumbnailDelegate.cpp
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

#include "NfThumbnailDelegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>

namespace NfDesktop {

enum CustomRoles {
        RatingRole = Qt::UserRole + 1,
        IsFlaggedRole = Qt::UserRole + 2
};

NfThumbnailDelegate::NfThumbnailDelegate(QObject* parent)
        : QStyledItemDelegate(parent) {}

QSize NfThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    [[maybe_unused]] const QModelIndex& index) const
{
        int w = option.decorationSize.width() + 16;
        int h = option.decorationSize.height() + 40;

        return QSize(w, h);
}

void NfThumbnailDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    // 1. Initialize option structure with item state (hover, selection, focus)
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 2. Prevent QStyle from drawing default icon/text (we will draw custom pixmap)
    opt.features &= ~QStyleOptionViewItem::HasDecoration;
    opt.features &= ~QStyleOptionViewItem::HasDisplay;

    // 3. Let QSS paint the item container (background-color, border, radius, margins)
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // 4. Calculate bounding area inside the QSS-styled item
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    int footerHeight = 30;
    QRect cardRect = opt.rect; // Driven by QSS padding/margin
    QRect imgBoundingBox(cardRect.left() + 6,
                         cardRect.top() + 6,
                         cardRect.width() - 12,
                         cardRect.height() - footerHeight - 8);

    auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

    if (!pixmap.isNull()) {
        auto scaledSize = pixmap.size().scaled(imgBoundingBox.size(), Qt::KeepAspectRatio);

        QRect imgRect(
            imgBoundingBox.left() + (imgBoundingBox.width() - scaledSize.width()) / 2,
            imgBoundingBox.top() + (imgBoundingBox.height() - scaledSize.height()) / 2,
            scaledSize.width(),
            scaledSize.height());

        QPainterPath imgClipPath;
        imgClipPath.addRoundedRect(imgRect, 4, 4);
        
        painter->setClipPath(imgClipPath);
        painter->drawPixmap(imgRect, pixmap);
    } else {
        // Draw placeholder background
        auto group = (opt.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive;
        painter->setPen(Qt::NoPen);
        painter->setBrush(opt.palette.color(group, QPalette::Base));
        painter->drawRoundedRect(imgBoundingBox, 4, 4);
    }

    painter->restore();
}

} // namespace NfDesktop
