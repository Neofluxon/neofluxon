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
#include "core/NfLogger.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>

namespace NfDesktop {

enum CustomRoles {
        RatingRole = Qt::UserRole + 1,
        IsFlaggedRole = Qt::UserRole + 2
};

NfThumbnailDelegate::NfThumbnailDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
        , m_padding{4}
{
}

QSize NfThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    [[maybe_unused]] const QModelIndex& index) const
{
        return option.decorationSize;
}

void NfThumbnailDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Prevent QStyle from drawing default icon/text
        opt.features &= ~QStyleOptionViewItem::HasDecoration;
        opt.features &= ~QStyleOptionViewItem::HasDisplay;

        // Let QSS paint the item container (background-color, border, radius, margins)
        auto* style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        auto thumbnailRect = QRect(opt.rect.left() + m_padding,
                                   opt.rect.top() + m_padding,
                                   opt.rect.width() - 2 * m_padding,
                                   opt.rect.height() - 2 * m_padding);

        auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
        if (!pixmap.isNull()) {
                auto scaledSize = pixmap.size().scaled(thumbnailRect.size(), Qt::KeepAspectRatio);
                QRect imgRect(thumbnailRect.left() + (thumbnailRect.width() - scaledSize.width()) / 2,
                              thumbnailRect.top() + (thumbnailRect.height() - scaledSize.height()) / 2,
                              scaledSize.width(),
                              scaledSize.height());

                QPainterPath imgClipPath;
                imgClipPath.addRoundedRect(imgRect, 2, 2);
                painter->setClipPath(imgClipPath);

                painter->drawPixmap(imgRect, pixmap);
        } else {
                // Draw placeholder background
                auto group = (opt.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive;
                painter->setPen(Qt::NoPen);
                painter->setBrush(opt.palette.color(group, QPalette::Base));
                painter->drawRoundedRect(thumbnailRect, 4, 4);
        }

        painter->restore();
}

} // namespace NfDesktop
