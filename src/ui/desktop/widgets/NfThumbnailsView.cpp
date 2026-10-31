/**
 * File name: NfThumbnailsView.cpp
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

#include "NfThumbnailsView.h"
#include "NfBrowserModel.h"
#include "NfThumbnailDelegate.h"
#include "core/NfLogger.h"

#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>

namespace NfDesktop {

NfThumbnailsView::NfThumbnailsView(QWidget* parent)
        : QListView(parent)
        , m_layoutMode{LayoutMode::GridView}
        , m_thumbnailSize{180, 120}

{
        setObjectName("NfThumbnailsView");
        setAttribute(Qt::WA_StyledBackground, true);

        setItemDelegate(new NfThumbnailDelegate(this));

        QObject::connect(verticalScrollBar(),
                         &QScrollBar::valueChanged,
                         this,
                         &NfThumbnailsView::onScrollChanged);
        QObject::connect(horizontalScrollBar(),
                         &QScrollBar::valueChanged,
                         this,
                         &NfThumbnailsView::onScrollChanged);

        setMouseTracking(true);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setUniformItemSizes(true);
        setSpacing(0);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        updateLayout();
}

void NfThumbnailsView::setLayoutMode(LayoutMode mode)
{
        if (m_layoutMode == mode)
                return;

        m_layoutMode = mode;
        updateLayout();
}

NfThumbnailsView::LayoutMode NfThumbnailsView::layoutMode() const
{
        return m_layoutMode;
}

void NfThumbnailsView::setThumbnailSize(const QSize &thumSize)
{
        m_thumbnailSize = thumSize;
        setIconSize(m_thumbnailSize);
        setGridSize(m_thumbnailSize);
}

const QSize& NfThumbnailsView::thumbnailSize() const
{
        return m_thumbnailSize;
}

void NfThumbnailsView::updateLayout()
{
        setViewMode(QListView::IconMode);
        setFlow(QListView::LeftToRight);
        setMovement(QListView::Static);
        setUniformItemSizes(true);

        if (m_layoutMode == LayoutMode::GridView) {
                setWrapping(true);
                setResizeMode(QListView::Adjust);
                setMinimumHeight(0);
                setMaximumHeight(QWIDGETSIZE_MAX);
                setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
                setWrapping(false);
                setResizeMode(QListView::Fixed);
                const int verticalPadding = 32;
                const int filmstripHeight = m_thumbnailSize.height() + verticalPadding;
                setMinimumHeight(filmstripHeight);
                setMaximumHeight(filmstripHeight);
                setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        setThumbnailSize(m_thumbnailSize);
}

void NfThumbnailsView::resizeEvent(QResizeEvent* event)
{
        QListView::resizeEvent(event);
}

void NfThumbnailsView::wheelEvent(QWheelEvent* event)
{
        QListView::wheelEvent(event);
}

void NfThumbnailsView::mouseMoveEvent(QMouseEvent* event)
{
        auto index = indexAt(event->pos());
        if (index.isValid())
                emit photoHovered(index);

        QListView::mouseMoveEvent(event);
}

void NfThumbnailsView::mouseDoubleClickEvent(QMouseEvent* event)
{
        auto index = indexAt(event->pos());

        if (index.isValid())
                emit photoActivated(index);

        QListView::mouseDoubleClickEvent(event);
}

void NfThumbnailsView::keyPressEvent(QKeyEvent *event)
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

void NfThumbnailsView::onScrollChanged()
{
        auto* browserModel = qobject_cast<NfBrowserModel*>(model());
        if (!browserModel)
                return;

        int firstVisibleRow = -1;
        int lastVisibleRow  = -1;
        const QRect viewportRect = viewport()->rect();

        // TODO: Find a way not to loop over model rows.
        for (int row = 0; row < model()->rowCount(); ++row) {
                const auto itemRect = visualRect(model()->index(row, 0));
                if (itemRect.intersects(viewportRect)) {
                        if (firstVisibleRow < 0)
                                firstVisibleRow = row;

                        lastVisibleRow = row;
                }
        }

        if (firstVisibleRow >= 0) {
                const int visibleCount = std::max(0, lastVisibleRow - firstVisibleRow + 1);
                browserModel->prefetchPage(firstVisibleRow,
                                           visibleCount,
                                           m_layoutMode == LayoutMode::GridView ? 1 : 4);
        }
}
} // namespace NfDesktop
