/**
 * File name: NfCentralWidget.h
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

#include "NfCentralWidget.h"
#include "NfContext.h"
#include "NfUiState.h"
#include "NfFolderModel.h"
#include "NfFolderView.h"
#include "NfLibraryView.h"

#include <QVBoxLayout>

using namespace NfUi;

namespace NfDesktop {

NfCentralWidget::NfCentralWidget(NfContext *ctx, QWidget* parent)
        : QWidget(parent)
        , m_context{ctx}
        , m_currentView{nullptr}
        , m_layout{new QVBoxLayout(this)}
        , m_browserModel{new NfBrowserModel(m_context, this));
        , m_folderModel{new NfFolderModel(m_context, this)}
        , m_libraryModel{new NfLibraryModel(m_context, this)}
{
        setObjectName("NfCentralWidget");

        m_layout->setContentsMargins(0, 0, 0, 0);
        setLayout(m_layout);

        QObject::connect(m_context->uiState,
                         &NfUiState::modeChanged,
                         this,
                         &NfCentralWidget::updateView);

        updateView();
}

void NfCentralWidget::showShootsView()
{
        //setCurrentView(new NfShootsModeView(this, m_shootsModel));
}

void NfCentralWidget::showFolderView()
{
        setCurrentView(new NfFolderView(NfFolderContext(m_context),
                                        m_folderModel,
                                        this));
}

void NfCentralWidget::showLibraryView()
{
        setCurrentView(new NfLibraryView(NfLibraryContext(m_context)));
}

void NfCentralWidget::setCurrentView(QWidget* view)
{
    if (m_currentView) {
            m_layout->removeWidget(m_currentView);
            m_currentView->deleteLater();
            m_currentView = nullptr;
    }

    if (view) {
            m_currentView = view;
            m_layout->addWidget(m_currentView);
            m_currentView->show();
    }
}

void NfCentralWidget::updateView()
{
        auto mode = m_context->uiState->mode();

        switch (mode) {
        case NfUiMode::Shoots:
                showShootsView();
                break;
        case NfUiMode::Folders:
                showFolderView();
                break;
        case NfUiMode::Library:
                showLibraryView();
                break;
        default:
                // Show no view
                setCurrentView(nullptr);
                break;
        }
}

} // namespace NfDesktop
