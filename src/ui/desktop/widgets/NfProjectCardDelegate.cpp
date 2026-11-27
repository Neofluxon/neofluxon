/**
 * File name: NfProjectCardDelegate.cpp
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

#include "NfProjectCardDelegate.h"
#include "NfProjectsModel.h"
#include "core/NfLogger.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QIcon>
#include <QMouseEvent>
#include <QFontMetrics>
#include <QTextOption>

#include <algorithm>

namespace NfDesktop {

NfProjectCardDelegate::NfProjectCardDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
{
}

bool NfProjectCardDelegate::editorEvent(QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index)
{
        return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void NfProjectCardDelegate::paint(QPainter* painter,
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
                auto scaledSize = pixmap.size().scaled(imageRect.size(),
                                                       Qt::KeepAspectRatio);
                QRect imgRect(imageRect.left()
                              + (imageRect.width() - scaledSize.width()) / 2,
                              imageRect.top()
                              + (imageRect.height() - scaledSize.height()) / 2,
                              scaledSize.width(),
                              scaledSize.height());

                QPainterPath imgClipPath;
                imgClipPath.addRoundedRect(imgRect, 10, 10);
                painter->setClipPath(imgClipPath);

                painter->drawPixmap(imgRect, pixmap);
        } else {
                // Draw placeholder background
                auto group = (opt.state & QStyle::State_Active)
                             ? QPalette::Normal : QPalette::Inactive;
                painter->setPen(Qt::NoPen);
                painter->setBrush(opt.palette.color(group, QPalette::Base));
                painter->drawRoundedRect(imageRect, 4, 4);
        }

        painter->restore();

        drawControls(painter, opt, index);
}

QRect NfProjectCardDelegate::thumbnailRect(const QRect& rect) const
{
        return rect.adjusted(m_padding,
                             m_padding,
                             -m_padding,
                             -m_padding - m_controlsHeight);
}

void NfProjectCardDelegate::drawControls(QPainter* painter,
                                         const QStyleOptionViewItem& opt,
                                         const QModelIndex& index) const
{
        painter->save();
        QRect controlsRect = opt.rect.adjusted(m_padding + 10,
                                               opt.rect.height() - m_controlsHeight,
                                               -m_padding,
                                               -m_padding);
        auto font = painter->font();
        font.setWeight(QFont::Bold);
        painter->setFont(font);
        painter->drawText(controlsRect.topLeft(),
                          index.data(Qt::DisplayRole).value<QString>());
        font.setPixelSize(10);
        font.setWeight(QFont::Normal);
        painter->setFont(font);
        painter->drawText(controlsRect.topLeft() + QPoint{0, 20},
                          "128 Photos  |  3 Nov 2026 | Archived");

        painter->restore();
}


} // namespace NfDesktop
