/**
 * File name: NfLeftPanel.cpp
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

#include "NfLeftPanel.h"
#include "NfContext.h"
#include "NfLibraryContext.h"
#include "NfUiState.h"
#include "NfUiFolderModeState.h"
#include "NfMainMenu.h"
#include "NfPathBrowser.h"
#include "NfLibraryTreeView.h"
#include "core/NfLogger.h"

#include <QVBoxLayout>
#include <QStackedWidget>

namespace NfDesktop {

        NfLeftPanel::NfLeftPanel(NfContext *ctx, QWidget *parent)
        : NfPanel(parent, NfPanel::PanelPosition::AlignLeft)
        , m_context{ctx}
        , m_pathBrowser{new NfPathBrowser(this)}
        , m_libraryTree{new NfLibraryTreeView(NfLibraryContext(ctx), this)}
        , m_stack{new QStackedWidget(this)}
{
        auto panelLayout = new QVBoxLayout(this);

        // Main menu
        auto mainMenu = new NfMainMenu(this);
        panelLayout->addWidget(mainMenu);

        m_stack->addWidget(m_pathBrowser);
        m_stack->addWidget(m_libraryTree);

        panelLayout->addWidget(m_stack);

        //panelLayout->addStretch();
        setLayout(panelLayout);

        QObject::connect(mainMenu, &NfMainMenu::shootsClicked, [this]() {
                m_context->uiState->setMode(NfUiMode::Shoots);
        });
        QObject::connect(mainMenu, &NfMainMenu::foldersClicked, [this]() {
                m_context->uiState->setMode(NfUiMode::Folders);
        });
        QObject::connect(mainMenu, &NfMainMenu::libraryClicked, [this]() {
                m_context->uiState->setMode(NfUiMode::Library);
        });
        QObject::connect(mainMenu, &NfMainMenu::libraryClicked, [this]() {
                m_context->uiState->setMode(NfUiMode::Library);
        });

        auto updateMode = [this, mainMenu](NfUiMode mode) {
                switch (mode) {
                case NfUiMode::Shoots:
                        mainMenu->setShootsMode();
                        break;
                case NfUiMode::Folders:
                        showPathBrowser();
                        mainMenu->setFolderMode();
                        break;
                case NfUiMode::Library:
                        showLibraryBrowser();
                        mainMenu->setLibraryMode();
                        break;
                default:
                        break;
                }
        };

        QObject::connect(m_context->uiState,
                         &NfUiState::modeChanged,
                         updateMode);

        QObject::connect(m_pathBrowser,
                         &NfPathBrowser::folderSelected,
                         m_context->uiState->folderModeState(),
                         &NfUiFolderModeState::setPath);
        QObject::connect(m_pathBrowser,
                         &NfPathBrowser::importRequested,
                         this,
                         &NfLeftPanel::showImportWidget);

        updateMode(m_context->uiState->mode());
}

void NfLeftPanel::showPathBrowser()
{
        m_stack->setCurrentWidget(m_pathBrowser);
}

void NfLeftPanel::showLibraryBrowser()
{
        NF_LOG_INFO("showlibrary");
        m_stack->setCurrentWidget(m_libraryTree);
        m_libraryTree->show();
}

void NfLeftPanel::showImportWidget(std::filesystem::path &path)
{
        m_context->library()->importFolder(path);
}

} // namespace NfDesktop
