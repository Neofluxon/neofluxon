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
#include "NfBrowserModel.h"
#include "core/NfLogger.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QIcon>
#include <QMouseEvent>
#include <QFontMetrics>
#include <algorithm>

namespace NfDesktop {

NfThumbnailDelegate::NfThumbnailDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
        , m_favoriteIcon{":/icons/favorite.svg"}
        , m_favoriteOutlineIcon{":/icons/favorite-outline.svg"}
        , m_ratingIcon{":/icons/rating.svg"}
        , m_ratingOutlineIcon{":/icons/rating-outline.svg"}
        , m_padding{4}
        , m_controlsHeight{32}
{
}

QSize NfThumbnailDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    [[maybe_unused]] const QModelIndex& index) const
{
        return option.decorationSize;
}

QRect NfThumbnailDelegate::thumbnailRect(const QRect& itemRect) const
{
        return itemRect.adjusted(m_padding,
                                 m_padding,
                                 -m_padding,
                                 -(m_controlsHeight + m_padding));
}

QRect NfThumbnailDelegate::controlsRect(const QRect& itemRect) const
{
        const auto thumbnail = thumbnailRect(itemRect);
        return QRect(thumbnail.left(),
                     thumbnail.bottom() + 5,
                     thumbnail.width(),
                     m_controlsHeight - 6);
}

QRect NfThumbnailDelegate::favoriteIconRect(const QRect& itemRect) const
{
        constexpr int iconSize = 16;
        constexpr int inset = 8;
        const auto controls = controlsRect(itemRect);
        return QRect(controls.right() - inset - iconSize + 1,
                     controls.top() + (controls.height() - iconSize) / 2,
                     iconSize,
                     iconSize);
}

QRect NfThumbnailDelegate::favoriteHitRect(const QRect& itemRect) const
{
        const auto icon = favoriteIconRect(itemRect);
        constexpr int hitSize = 28;
        return QRect(icon.center().x() - hitSize / 2,
                     icon.center().y() - hitSize / 2,
                     hitSize,
                     hitSize);
}

QRect NfThumbnailDelegate::ratingHitRect(const QRect& itemRect, int rating) const
{
        constexpr int starSize = 14;
        constexpr int starSpacing = 1;
        const auto controls = controlsRect(itemRect);
        const int index = std::clamp(rating - 1, 0, 4);
        return QRect(controls.left() + 8 + index * (starSize + starSpacing),
                     controls.top() + (controls.height() - starSize) / 2,
                     18,
                     controls.height());
}

void NfThumbnailDelegate::setFavoriteHoverIndex(const QModelIndex& index)
{
        m_favoriteHoverIndex = index;
}

bool NfThumbnailDelegate::editorEvent(QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index)
{
        if (!model || !index.isValid()
            || event->type() != QEvent::MouseButtonRelease)
                return QStyledItemDelegate::editorEvent(event, model, option, index);

        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        const auto photo = model->data(index, NfBrowserModel::PhotoRole).value<NfPhoto>();
        if (photo.id().isValid()) {
                for (int rating = 1; rating <= 5; ++rating) {
                        if (ratingHitRect(option.rect, rating).contains(mouseEvent->pos())) {
                                if (mouseEvent->button() == Qt::LeftButton)
                                        emit ratingChanged(photo.id(), rating);
                                return true;
                        }
                }
        }

        if (mouseEvent->button() != Qt::LeftButton
            || !favoriteHitRect(option.rect).contains(mouseEvent->pos()))
                return QStyledItemDelegate::editorEvent(event, model, option, index);

        if (!photo.id().isValid())
                return true;

        emit favoriteToggleRequested(photo.id(), !photo.isFavorite());

        return true;
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

        const auto imageRect = thumbnailRect(opt.rect);

        auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
        if (!pixmap.isNull()) {
                auto scaledSize = pixmap.size().scaled(imageRect.size(), Qt::KeepAspectRatio);
                QRect imgRect(imageRect.left() + (imageRect.width() - scaledSize.width()) / 2,
                              imageRect.top() + (imageRect.height() - scaledSize.height()) / 2,
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
                painter->drawRoundedRect(imageRect, 4, 4);
        }

        painter->restore();

        const auto photo = index.data(NfBrowserModel::PhotoRole).value<NfPhoto>();
        const auto controls = controlsRect(opt.rect);

        painter->save();
        constexpr int starSize = 14;
        constexpr int starSpacing = 1;
        for (int i = 0; i < 5; ++i) {
                const QRect starRect(controls.left() + 8 + i * (starSize + starSpacing),
                                     controls.top() + (controls.height() - starSize) / 2,
                                     starSize,
                                     starSize);
                if (i < std::clamp(photo.rating(), 0, 5))
                        m_ratingIcon.paint(painter, starRect);
                else
                        m_ratingOutlineIcon.paint(painter, starRect);
        }

        painter->restore();

        painter->save();
        const auto favIconRect = favoriteIconRect(opt.rect);
        const auto iconRect = favIconRect;
        if (photo.isFavorite()) {
                m_favoriteIcon.paint(painter, iconRect);
        } else {
                if (index == m_favoriteHoverIndex)
                        painter->setOpacity(1.0);
                else
                        painter->setOpacity(0.78);
                m_favoriteOutlineIcon.paint(painter, iconRect);
        }
        painter->restore();
}

} // namespace NfDesktop
