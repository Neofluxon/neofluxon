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
#include <QMouseEvent>
#include <QFontMetrics>
#include <QTextOption>
#include <QDateTime>
#include <QLocale>
#include <QPixmap>

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

        drawControls(painter, opt, index, imageRect);
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
                                         const QModelIndex& index,
                                         const QRect &imageRect) const
{
        painter->save();
        auto controlsHeight = opt.rect.height() - 2 * m_padding - imageRect.height();
        QRect controlsRect(QPoint(opt.rect.left() + m_padding, imageRect.bottom()),
                           QSize{opt.rect.width() - 2 * m_padding, controlsHeight});
        controlsRect.adjust(10, 2, -10, -2);

        // Draw project name
        auto font = painter->font();
        font.setWeight(QFont::Bold);
        painter->setFont(font);
        auto nameTextHeight = QFontMetrics(painter->font()).height();
        auto nameTextPos = controlsRect.topLeft() + QPoint(0, nameTextHeight);
        painter->drawText(nameTextPos,
                          index.data(Qt::DisplayRole).value<QString>());

        font.setPixelSize(10);
        font.setWeight(QFont::Normal);
        painter->setFont(font);

        // The position of the porject info
        auto infoRect = QRect(nameTextPos + QPoint(0, 5),
                              QSize{controlsRect.width(), 30});

        auto drawLabel = [painter, infoRect](int xPos,
                                             const QPixmap &icon,
                                             const QString &text) -> int {
                QRect iconRect(QPoint(xPos, infoRect.top()
                                      + (infoRect.height() - icon.height()) / 2),
                               icon.size());

                painter->drawPixmap(iconRect, icon);

                if (text.isEmpty())
                        return iconRect.right();

                auto labelRect = iconRect;
                labelRect.translate(labelRect.width() + 3, 1);
                labelRect.setWidth(QFontMetrics(painter->font()).horizontalAdvance(text));
                painter->drawText(labelRect,
                                  Qt::AlignVCenter | Qt::AlignLeft,
                                  text);

                return labelRect.right();
        };

        auto labelPos = infoRect.left();
        auto icon = QPixmap{":/icons/picture.svg"};
        auto numberOfPhotos = index.data(NfProjectsModel::NumberOfPhotosRole).value<int>();
        labelPos = drawLabel(labelPos,
                             icon.scaled(QSize(16, 16),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation),
                             QString("%1 %2").arg(numberOfPhotos).arg(tr(" Photos")));
        labelPos = drawLabel(labelPos + 5,
                             QPixmap{":/icons/v-separator.svg"},
                             "");
        icon = QPixmap{":/icons/calendar.svg"};
        auto modifiedDate = index.data(NfProjectsModel::ModifiedRole).value<QDateTime>();
        drawLabel(labelPos,
                  icon.scaled(QSize(18, 18),
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation),
                  QLocale().toString(modifiedDate.date(), "dd MMM"));

        painter->restore();
}


} // namespace NfDesktop
