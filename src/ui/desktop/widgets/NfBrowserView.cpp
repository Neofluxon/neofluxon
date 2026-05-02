/**
 * File name: NfBrowserView.cpp
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

#include "NfBrowserView.h"
#include "NfBrowserModel.h"

#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>

namespace NfDesktop {

NfBrowserView::NfBrowserView(QWidget* parent)
        : QListView(parent)
        , m_layoutMode{LayoutMode::GridView}
        , m_thumbnailSize{42}

{
        setObjectName("NfBrowserView");

        connect(verticalScrollBar(),
                &QScrollBar::valueChanged,
                this,
                &NfBrowserView::onScrollChanged);

        setMouseTracking(true);

        setSelectionMode(QAbstractItemView::SingleSelection);

        setUniformItemSizes(true);
        setSpacing(0);

        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        updateLayout();
}

void NfBrowserView::setLayoutMode(LayoutMode mode)
{
        if (m_layoutMode == mode)
                return;

        m_layoutMode = mode;
        updateLayout();
}

NfBrowserView::LayoutMode NfBrowserView::layoutMode() const
{
        return m_layoutMode;
}

void NfBrowserView::setThumbnailSize(int size)
{
        m_thumbnailSize = size;
        setIconSize(QSize(size, size));
}

int NfBrowserView::thumbnailSize() const
{
        return m_thumbnailSize;
}

void NfBrowserView::updateLayout()
{
        setViewMode(QListView::IconMode);
        setFlow(QListView::LeftToRight);
        if (m_layoutMode == LayoutMode::GridView) {
                setWrapping(true);
                setResizeMode(QListView::Adjust);
        } else {
                setWrapping(false);
        }
}

void NfBrowserView::resizeEvent(QResizeEvent* event)
{
        QListView::resizeEvent(event);
}

void NfBrowserView::wheelEvent(QWheelEvent* event)
{
        QListView::wheelEvent(event);
}

void NfBrowserView::mouseMoveEvent(QMouseEvent* event)
{
        auto index = indexAt(event->pos());
        if (index.isValid())
                emit photoHovered(index);

        QListView::mouseMoveEvent(event);
}

void NfBrowserView::mouseDoubleClickEvent(QMouseEvent* event)
{
        auto index = indexAt(event->pos());

        if (index.isValid())
                emit photoActivated(index);

        QListView::mouseDoubleClickEvent(event);
}

void NfBrowserView::keyPressEvent(QKeyEvent *event)
{
        QListView::keyPressEvent(event);

        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
                {
                        auto index = currentIndex();
                        if (index.isValid())
                                scrollTo(index, QAbstractItemView::PositionAtCenter);
                        event->accept();
                        break;
                }
        default:
                break;
        }
}

void NfBrowserView::onScrollChanged()
{
        auto* browserModel = qobject_cast<NfBrowserModel*>(model());
        if (!browserModel)
                return;

        // Top visible item
        QModelIndex top = indexAt(QPoint(0, 0));
        int firstVisibleRow = top.isValid() ? top.row() : 0;

        // Bottom visible item (more reliable than grid math)
        QModelIndex bottom = indexAt(QPoint(0, viewport()->height() - 1));
        int lastVisibleRow = bottom.isValid()
                ? bottom.row()
                : model()->rowCount() - 1;

        int visibleCount = lastVisibleRow - firstVisibleRow + 1;
        if (visibleCount <= 0)
                visibleCount = 1;

        browserModel->prefetchPage(firstVisibleRow, visibleCount);
}

} // namespace NfDesktop
