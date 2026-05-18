/**
 * File name: NfFolderActionDelegate.cpp
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

#include "NfFolderActionDelegate.h"
#include "core/NfLogger.h"

#include <QPainter>
#include <QMouseEvent>
#include <QFileSystemModel>
#include <QFile>

namespace NfDesktop {

NfFolderActionDelegate::NfFolderActionDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
        , m_plusIcon(":/icons/plus.png")
        , m_btnWidth{8}
{
}

QRect NfFolderActionDelegate::getButtonRect(const QStyleOptionViewItem& option) const
{
        int x = option.rect.right() - m_btnWidth - m_btnPadding;
        int y = option.rect.top() + (option.rect.height() - m_btnWidth) / 2;
        return QRect(x, y, m_btnWidth, m_btnWidth);
}

void NfFolderActionDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Prevent the text from overlapping the button placement area on selection
        if (opt.state & QStyle::State_Selected)
                opt.rect.setRight(opt.rect.right() - (m_btnWidth + m_btnPadding * 2));

        // Draw the standard file system icon and folder label first
        QStyledItemDelegate::paint(painter, opt, index);

        // Contextual Action: Render the cached asset
        // icon only when the row is selected.
        if (option.state & QStyle::State_Selected) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);

                auto btnRect = getButtonRect(option);

                // Draw background base for the icon
                painter->setPen(Qt::NoPen);
                painter->setBrush(option.palette.accent());
                painter->drawRoundedRect(btnRect, 4, 4);

                // Add slight padding so the icon graphic sits
                // comfortably inside the button boundaries
                auto iconRect = btnRect.adjusted(4, 4, -4, -4);
                m_plusIcon.paint(painter,
                                 iconRect,
                                 Qt::AlignCenter,
                                 QIcon::Normal,
                                 QIcon::On);
                //                NF_LOG_DEBUG("called");

                painter->restore();
        }
}

bool NfFolderActionDelegate::editorEvent(QEvent* event,
                                         QAbstractItemModel* model,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index)
{
        if (event->type() != QEvent::MouseButtonPress
            && event->type() != QEvent::MouseButtonRelease) {
                return QStyledItemDelegate::editorEvent(event,
                                                        model,
                                                        option,
                                                        index);
        }

        if (option.state & QStyle::State_Selected) {
                auto* mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton
                    && getButtonRect(option).contains(mouseEvent->pos())) {
                        if (event->type() == QEvent::MouseButtonRelease) {
                                auto fsModel = qobject_cast<const QFileSystemModel*>(model);
                                if (fsModel)
                                        emit importRequested(fsModel->filePath(index));
                        }
                        return true;
                }
        }

        return QStyledItemDelegate::editorEvent(event, model, option, index);
}

} // namespace NfDesktop
