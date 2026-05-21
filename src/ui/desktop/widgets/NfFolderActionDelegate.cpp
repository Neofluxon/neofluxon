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

#include <QAbstractItemView>
#include <QMouseEvent>
#include <QPainter>
#include <QFileSystemModel>

namespace NfDesktop {

NfFolderActionDelegate::NfFolderActionDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
        , m_plusIcon(":/icons/plus.svg")
        , m_btnWidth{24}
        , m_btnPadding{4}
{
        auto* view = qobject_cast<QAbstractItemView*>(parent);
        if (view) {
                view->viewport()->setMouseTracking(true);
                view->viewport()->installEventFilter(this);
        }
}

QRect NfFolderActionDelegate::getButtonRect(
        const QStyleOptionViewItem& option) const
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

        opt.rect.setRight(opt.rect.right() - (m_btnWidth + m_btnPadding * 2));

        QStyledItemDelegate::paint(painter, opt, index);

        if (!index.isValid())
                return;

        bool isSelected = (option.state & QStyle::State_Selected);
        bool isRowHovered = (index == m_hoveredIndex);

        if (!isSelected)
                return;

        // Determine if the mouse cursor is directly interacting with the button coordinates
        bool btnHovered = (isRowHovered && m_buttonHovered);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QRect btnRect = getButtonRect(option);

        painter->setPen(Qt::NoPen);
        painter->setBrush(btnHovered ? option.palette.highlight()
                                     : option.palette.accent());

        painter->drawRoundedRect(btnRect, 4, 4);

        QRect iconRect = btnRect.adjusted(4, 4, -4, -4);
        m_plusIcon.paint(painter,
                         iconRect,
                         Qt::AlignCenter,
                         btnHovered ? QIcon::Active
                                    : QIcon::Normal,
                         QIcon::On);

        painter->restore();
}

bool NfFolderActionDelegate::eventFilter(QObject* object,
                                         QEvent* event)
{
        auto* view = qobject_cast<QAbstractItemView*>(parent());
        if (!view || object != view->viewport())
                return false;

        if (event->type() == QEvent::MouseMove) {
                auto* mouseEvent = static_cast<QMouseEvent*>(event);
                QModelIndex index = view->indexAt(mouseEvent->pos());

                QModelIndex oldIndex = m_hoveredIndex;
                bool oldButtonHovered = m_buttonHovered;

                m_hoveredIndex = index;
                m_buttonHovered = false;

                if (index.isValid()) {
                        QStyleOptionViewItem opt;
                        opt.rect = view->visualRect(index);
                        QRect btnRect = getButtonRect(opt);

                        m_buttonHovered = btnRect.contains(mouseEvent->pos());
                }

                // FIX: Redraw if row changes OR if mouse enters/leaves the plus icon bounds
                if (oldIndex != m_hoveredIndex || oldButtonHovered != m_buttonHovered) {
                        if (oldIndex.isValid())
                                view->update(oldIndex);

                        if (m_hoveredIndex.isValid())
                                view->update(m_hoveredIndex);
                }

                return false;
        }

        if (event->type() == QEvent::Leave) {
                QModelIndex old = m_hoveredIndex;

                m_hoveredIndex = QModelIndex();
                m_buttonHovered = false;

                if (old.isValid())
                        view->update(old);

                return false;
        }

        return false;
}

bool NfFolderActionDelegate::editorEvent(
        QEvent* event,
        QAbstractItemModel* model,
        const QStyleOptionViewItem& option,
        const QModelIndex& index)
{
        if (event->type() == QEvent::MouseButtonRelease) {
                auto* mouseEvent = static_cast<QMouseEvent*>(event);

                if (mouseEvent->button() == Qt::LeftButton
                    && getButtonRect(option).contains(mouseEvent->pos())) {

                        auto* fsModel = qobject_cast<QFileSystemModel*>(model);
                        if (fsModel)
                                emit importRequested(fsModel->filePath(index));

                        return true;
                }
        }

        return QStyledItemDelegate::editorEvent(event, model, option, index);
}

} // namespace NfDesktop
