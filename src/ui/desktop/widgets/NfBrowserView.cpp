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
#include "NfUiBrowserState.h"
#include "NfBrowserModel.h"
#include "NfBreadcrumbView.h"
#include "NfThumbnailsView.h"
#include "NfPhotoPreviewView.h"

#include <QVBoxLayout>
#include <QKeyEvent>

using namespace NfUi;

namespace NfDesktop {

NfBrowserView::NfBrowserView(NfUiBrowserState *state,
                             NfBrowserModel *model,
                             QWidget* parent)
        : QWidget(parent)
        , m_state{state}
        , m_model{model}
{
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(0);

        m_breadcrumbBar = new NfBreadcrumbView(this);
        m_breadcrumbBar->setObjectName("BrowserBreadcrumb");
        m_mainLayout->addWidget(m_breadcrumbBar);

        m_thumbnailsView = new NfThumbnailsView(this);
        m_thumbnailsView->setModel(m_model);
        m_mainLayout->addWidget(m_thumbnailsView);

        QObject::connect(m_state,
                         &NfUiBrowserState::viewModeChanged,
                         this,
                         &NfBrowserView::updateView);
        QObject::connect(m_thumbnailsView, &QListView::doubleClicked,
                         [this](const QModelIndex &index) {
                                 m_state->setViewMode(NfUiBrowserState::ViewMode::Preview);
                         });

        updateView();
}

void NfBrowserView::showGridView()
{
        m_thumbnailsView->setLayoutMode(NfThumbnailsView::LayoutMode::GridView);

        if (m_photoPreviewView) {
                m_mainLayout->removeWidget(m_photoPreviewView);
                m_photoPreviewView->deleteLater();
                m_photoPreviewView = nullptr;
        }

        m_mainLayout->setStretch(0, 1);
}

void NfBrowserView::showPreviewView()
{
        m_thumbnailsView->setLayoutMode(NfThumbnailsView::LayoutMode::FilmstripView);

        if (!m_photoPreviewView) {
                m_photoPreviewView = new NfPhotoPreviewView(m_model, this);
                QObject::connect(m_thumbnailsView->selectionModel(),
                                 &QItemSelectionModel::currentChanged,
                                 m_photoPreviewView,
                                 &NfPhotoPreviewView::setPhotoIndex);

                // Insert at the top
                m_mainLayout->insertWidget(0, m_photoPreviewView);

                // Index 0 (Preview) gets more space, Index 1 (Filmstrip) gets less
                m_mainLayout->setStretch(0, 7);
                m_mainLayout->setStretch(1, 3);

                m_photoPreviewView->setPhotoIndex(m_thumbnailsView->currentIndex());
                m_thumbnailsView->scrollTo(m_thumbnailsView->currentIndex(),
                                        QAbstractItemView::PositionAtCenter);
        }
}

void NfBrowserView::updateView()
{
        switch(m_state->viewMode()) {
        case NfUiBrowserState::ViewMode::Preview:
                showPreviewView();
                break;
        case NfUiBrowserState::ViewMode::Grid:
        default:
                showGridView();
                break;
        }
}

void NfBrowserView::keyPressEvent(QKeyEvent *event)
{
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
                        m_thumbnailsView->setFocus();

                        // Forward the event to the browser view.
                        QCoreApplication::sendEvent(m_thumbnailsView, event);

                        event->accept();
                        break;
                }

        default:
                QWidget::keyPressEvent(event);
                break;
        }
}

} // namespace NfDesktop
