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
#include "NfDesktopContext.h"
#include "NfUiState.h"
#include "core/NfLogger.h"

#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QShowEvent>

namespace NfDesktop {

NfThumbnailsView::NfThumbnailsView(const NfDesktopContext& context, QWidget* parent)
        : QListView(parent)
        , m_layoutMode{LayoutMode::GridView}
        , m_thumbnailSize{180, 120}
        , m_hasControls{context.uiState()->mode() == NfUi::NfUiMode::Library}

{
        setObjectName("NfThumbnailsView");
        setAttribute(Qt::WA_StyledBackground, true);

        m_thumbnailDelegate = new NfThumbnailDelegate(this);
        m_thumbnailDelegate->setHasControls(m_hasControls);
        setItemDelegate(m_thumbnailDelegate);
        QObject::connect(m_thumbnailDelegate,
                         &NfThumbnailDelegate::favoriteToggleRequested,
                         this,
                         &NfThumbnailsView::favoriteToggleRequested);
        QObject::connect(m_thumbnailDelegate,
                         &NfThumbnailDelegate::ratingChanged,
                         this,
                         &NfThumbnailsView::ratingChanged);

        QObject::connect(verticalScrollBar(),
                         &QScrollBar::valueChanged,
                         this,
                         &NfThumbnailsView::onScrollChanged);
        QObject::connect(horizontalScrollBar(),
                         &QScrollBar::valueChanged,
                         this,
                         &NfThumbnailsView::onScrollChanged);
        QObject::connect(horizontalScrollBar(),
                         &QScrollBar::rangeChanged,
                         this,
                         [this]() { updateFilmstripHeight(); });

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

        // Save the current scoll if we are moving to filmstrip mode
        if (mode == LayoutMode::FilmstripView) {
                m_gridScrollPosition = verticalScrollBar()->value();
                m_restoreGridScrollPosition = true;
        }

        m_layoutMode = mode;

        updateLayout();
}

NfThumbnailsView::LayoutMode NfThumbnailsView::layoutMode() const
{
        return m_layoutMode;
}

void NfThumbnailsView::restoreGridScrollPosition()
{
        if (!m_restoreGridScrollPosition
            || m_layoutMode != LayoutMode::GridView)
                return;

        verticalScrollBar()->setValue(m_gridScrollPosition);
        m_restoreGridScrollPosition = false;
}

void NfThumbnailsView::setThumbnailSize(const QSize &thumSize)
{
        m_thumbnailSize = thumSize;
        setIconSize(m_thumbnailSize);
        m_thumbnailDelegate->setThumbnailSize(m_thumbnailSize);

        auto itemSize = m_thumbnailSize;
        if (m_hasControls)
                itemSize.rheight() += m_thumbnailDelegate->controlsHeight();
        setGridSize(itemSize);

        updateFilmstripHeight();
}

void NfThumbnailsView::mousePressEvent(QMouseEvent* event)
{
        m_favoritePressActive = false;
        m_favoritePressIndex = QModelIndex();
        m_ratingPressActive = false;
        m_ratingPressIndex = QModelIndex();

        if (event->button() == Qt::LeftButton) {
                const auto index = indexAt(event->pos());
                auto* delegate = thumbnailDelegate();
                if (index.isValid() && delegate
                    && delegate->favoriteHitRect(visualRect(index)).contains(event->pos())) {
                        m_favoritePressActive = true;
                        m_favoritePressIndex = index;
                        event->accept();
                        return;
                }

                if (index.isValid() && delegate) {
                        for (int rating = 1; rating <= 5; ++rating) {
                                if (delegate->ratingHitRect(visualRect(index), rating)
                                    .contains(event->pos())) {
                                        m_ratingPressActive = true;
                                        m_ratingPressIndex = index;
                                        event->accept();
                                        return;
                                }
                        }
                }
        }

        QListView::mousePressEvent(event);
}

void NfThumbnailsView::mouseReleaseEvent(QMouseEvent* event)
{
        if (m_ratingPressActive && event->button() == Qt::LeftButton) {
                m_ratingPressActive = false;

                auto* delegate = thumbnailDelegate();
                if (delegate && m_ratingPressIndex.isValid()) {
                        QStyleOptionViewItem option;
                        option.rect = visualRect(m_ratingPressIndex);
                        option.widget = this;
                        delegate->editorEvent(event,
                                              model(),
                                              option,
                                              m_ratingPressIndex);
                }

                m_ratingPressIndex = {};
                event->accept();

                return;
        }

        if (m_favoritePressActive && event->button() == Qt::LeftButton) {
                m_favoritePressActive = false;

                auto* delegate = thumbnailDelegate();
                if (delegate && m_favoritePressIndex.isValid()
                    && delegate->favoriteHitRect(visualRect(m_favoritePressIndex)).contains(event->pos())) {
                        QStyleOptionViewItem option;
                        option.rect = visualRect(m_favoritePressIndex);
                        option.widget = this;
                        delegate->editorEvent(event, model(), option, m_favoritePressIndex);
                }

                m_favoritePressIndex = {};
                event->accept();

                return;
        }

        QListView::mouseReleaseEvent(event);
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
                setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        setThumbnailSize(m_thumbnailSize);
}

NfThumbnailDelegate* NfThumbnailsView::thumbnailDelegate() const
{
        return m_thumbnailDelegate;
}

void NfThumbnailsView::updateFilmstripHeight()
{
        if (m_layoutMode != LayoutMode::FilmstripView)
                return;

        const auto* scrollBar = horizontalScrollBar();
        // Add 3 pixel padding above and below the toolbar
        // when the toolbar is visible
        const int scrollBarHeight = scrollBar->isVisible()
                ? scrollBar->height() + 6
                : 0;
        const auto margins = viewportMargins();
        const int viewportOverhead = 2 * frameWidth()
                + margins.top() + margins.bottom();
        const int filmstripContentHeight = m_thumbnailSize.height()
                + (m_hasControls ? m_thumbnailDelegate->controlsHeight() : 0);
        const int filmstripHeight = filmstripContentHeight
                + viewportOverhead + scrollBarHeight;
        if (minimumHeight() != filmstripHeight
            || maximumHeight() != filmstripHeight) {
                setMinimumHeight(filmstripHeight);
                setMaximumHeight(filmstripHeight);
        }
}

void NfThumbnailsView::showEvent(QShowEvent* event)
{
        QListView::showEvent(event);
        updateFilmstripHeight();
}

void NfThumbnailsView::resizeEvent(QResizeEvent* event)
{
        QListView::resizeEvent(event);
        updateFilmstripHeight();
}

void NfThumbnailsView::wheelEvent(QWheelEvent* event)
{
        QListView::wheelEvent(event);
}

void NfThumbnailsView::mouseMoveEvent(QMouseEvent* event)
{
        const auto index = indexAt(event->pos());
        if (index.isValid())
                emit photoHovered(index);

        auto* delegate = thumbnailDelegate();
        QModelIndex nextHoverIndex;
        QModelIndex nextRatingHoverIndex;
        int nextRatingHover = 0;
        bool ratingControlHovered = false;
        if (index.isValid() && delegate) {
                if (delegate->favoriteHitRect(visualRect(index)).contains(event->pos()))
                        nextHoverIndex = index;

                for (int rating = 1; rating <= 5; ++rating) {
                        if (delegate->ratingHitRect(visualRect(index), rating)
                            .contains(event->pos())) {
                                ratingControlHovered = true;
                                nextRatingHoverIndex = index;
                                nextRatingHover = rating;
                                break;
                        }
                }
        }

        if (nextHoverIndex.isValid() || ratingControlHovered)
                setCursor(Qt::PointingHandCursor);
        else
                setCursor(Qt::ArrowCursor);

        if (nextRatingHoverIndex != m_ratingHoverIndex
            || nextRatingHover != m_ratingHover) {
                if (m_ratingHoverIndex.isValid())
                        viewport()->update(visualRect(m_ratingHoverIndex));

                m_ratingHoverIndex = nextRatingHoverIndex;
                m_ratingHover = nextRatingHover;
                if (delegate)
                        delegate->setRatingHover(m_ratingHoverIndex, m_ratingHover);

                if (m_ratingHoverIndex.isValid())
                        viewport()->update(visualRect(m_ratingHoverIndex));
        }

        if (nextHoverIndex != m_favoriteHoverIndex) {
                if (m_favoriteHoverIndex.isValid()) {
                        viewport()->update(visualRect(m_favoriteHoverIndex));
                        if (auto* oldDelegate = thumbnailDelegate()) {
                                oldDelegate->setFavoriteHoverIndex(QModelIndex());
                        }
                }

                m_favoriteHoverIndex = nextHoverIndex;
                if (m_favoriteHoverIndex.isValid()) {
                        if (auto* hoverDelegate = thumbnailDelegate()) {
                                hoverDelegate->setFavoriteHoverIndex(m_favoriteHoverIndex);
                        }
                        viewport()->update(visualRect(m_favoriteHoverIndex));
                }
        }

        QListView::mouseMoveEvent(event);
}

void NfThumbnailsView::leaveEvent(QEvent* event)
{
        auto* delegate = thumbnailDelegate();
        if (m_ratingHoverIndex.isValid()) {
                viewport()->update(visualRect(m_ratingHoverIndex));
                m_ratingHoverIndex = {};
                m_ratingHover = 0;
                if (delegate)
                        delegate->setRatingHover({}, 0);
        }

        if (m_favoriteHoverIndex.isValid()) {
                viewport()->update(visualRect(m_favoriteHoverIndex));
                m_favoriteHoverIndex = {};
                if (delegate)
                        delegate->setFavoriteHoverIndex({});
        }

        QListView::leaveEvent(event);
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
        if (event->key() == Qt::Key_V && event->modifiers() == Qt::NoModifier) {
                emit viewModeToggleRequested();
                event->accept();
                return;
        }

        if (event->key() == Qt::Key_Escape
            && event->modifiers() == Qt::NoModifier
            && m_layoutMode == LayoutMode::FilmstripView) {
                emit gridViewRequested();
                event->accept();
                return;
        }

        if (event->key() == Qt::Key_S && event->modifiers() == Qt::NoModifier) {
                emit sortDirectionToggleRequested();
                event->accept();
                return;
        }

        if (event->key() == Qt::Key_S && event->modifiers() == Qt::ShiftModifier) {
                emit sortFieldCycleRequested();
                event->accept();
                return;
        }

        QListView::keyPressEvent(event);

        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
                processEnterKey(event);
                break;
        case Qt::Key_F:
                processFavoriteKey(event);
                break;
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
                processRatingKey(event);
                break;
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

void NfThumbnailsView::processEnterKey(QKeyEvent* event)
{
        if (event->modifiers() != Qt::NoModifier)
                return;

        const auto index = currentIndex();
        if (!index.isValid())
                return;

        emit photoActivated(index);
        event->accept();
}

void NfThumbnailsView::processFavoriteKey(QKeyEvent* event)
{
        if (event->modifiers() != Qt::NoModifier)
                return;

        const auto index = currentIndex();
        if (!index.isValid())
                return;

        auto role = NfBrowserModel::PhotoRole;
        const auto photo = model()->data(index, role).value<NfPhoto>();
        if (!photo.id().isValid())
                return;

        emit favoriteToggleRequested(photo.id(),
                                     !photo.isFavorite());
        event->accept();
}

void NfThumbnailsView::processRatingKey(QKeyEvent* event)
{
        if (event->modifiers() != Qt::NoModifier)
                return;

        const auto index = currentIndex();
        if (!index.isValid())
                return;

        const auto photo = model()->data(
                index, NfBrowserModel::PhotoRole).value<NfPhoto>();
        if (!photo.id().isValid())
                return;

        const int rating = event->key() - Qt::Key_0;
        emit ratingChanged(photo.id(), rating);
        event->accept();
}

} // namespace NfDesktop
