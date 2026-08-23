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
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        // Card boundary with margin relative to grid slot
        auto cardRect = option.rect.adjusted(3, 3, -3, -3);

        bool isSelected = option.state & QStyle::State_Selected;
        bool isHovered  = option.state & QStyle::State_MouseOver;

        // --- 1. Fetch Dynamic Palette Colors from QSS / QStyle ---
        auto group = (option.state & QStyle::State_Active)
                ? QPalette::Normal
                : QPalette::Inactive;

        // Fill background using QSS Selection / AlternateBase / Button color roles
        QColor bgColor;
        if (isSelected) {
                bgColor = option.palette.color(group, QPalette::Highlight);
        } else if (isHovered) {
                bgColor = option.palette.color(group, QPalette::Midlight);
        } else {
                bgColor = option.palette.color(group, QPalette::Button);
        }

        // Pen color driven by Highlighting/Link or WindowText roles
        QColor borderColor = isSelected ? option.palette.color(group, QPalette::Highlight)
                : isHovered  ? option.palette.color(group, QPalette::Link)
                : option.palette.color(group, QPalette::Dark);

        // --- 2. Draw Card Container ---
        QPainterPath cardPath;
        cardPath.addRoundedRect(cardRect, 6, 6);
        painter->fillPath(cardPath, bgColor);

        QPen borderPen(borderColor, isSelected ? 2 : 1);
        painter->setPen(borderPen);
        painter->drawPath(cardPath);

        // --- 3. Image Area Setup ---
        int footerHeight = 30;
        QRect imgBoundingBox(cardRect.left() + 6,
                             cardRect.top() + 6,
                             cardRect.width() - 12,
                             cardRect.height() - footerHeight - 8);

        auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

        if (!pixmap.isNull()) {
                // Fit aspect ratio inside the bounding box
                auto scaledSize = pixmap.size().scaled(imgBoundingBox.size(),
                                                       Qt::KeepAspectRatio);

                // Center pixmap inside the image box
                QRect imgRect(
                              imgBoundingBox.left() + (imgBoundingBox.width() - scaledSize.width()) / 2,
                              imgBoundingBox.top() + (imgBoundingBox.height() - scaledSize.height()) / 2,
                              scaledSize.width(),
                              scaledSize.height());

                // Clip image corners slightly if it hits top corners
                QPainterPath imgClipPath;
                imgClipPath.addRoundedRect(imgRect, 4, 4);
                painter->save();
                painter->setClipPath(imgClipPath);
                painter->drawPixmap(imgRect, pixmap);
                painter->restore();
        } else {
                // Placeholder background drawn using the Base palette role from QSS
                painter->setPen(Qt::NoPen);
                painter->setBrush(option.palette.color(group, QPalette::Base));
                painter->drawRoundedRect(imgBoundingBox, 4, 4);
        }

        painter->restore();
}

} // namespace NfDesktop
