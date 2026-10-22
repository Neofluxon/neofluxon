/**
 * File name: NfTopBar.cpp
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

#include "NfTopBar.h"
#include "NfBrowserViewModeBar.h"
#include "NfContext.h"
#include "NfSettingsDialog.h"
#include "NfLibraryAdapter.h"
#include "core/library/NfLibrary.h"
#include "NfUiShootsModeState.h"
#include "NfUiFolderModeState.h"
#include "NfUiLibraryModeState.h"
#include "NfUiBrowserState.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QToolButton>
#include <QString>

using namespace NfUi;

namespace NfDesktop {

NfTopBar::NfTopBar(NfContext* ctx, QWidget *parent)
	: NfStyledWidget(parent)
        , m_context{ctx}
{
	setObjectName("NfTopBar");
        setFixedHeight(48);

	auto topBarLayout = new QHBoxLayout(this);
        topBarLayout->setContentsMargins(0, 0, 0, 0);
        topBarLayout->setSpacing(3);

        // Logo
        QIcon logoIcon(":/logo.svg");
        auto logoLabel = new QLabel(this);
        logoLabel->setObjectName("NfLogo");
        logoLabel->setPixmap(logoIcon.pixmap(24, 24));
        logoLabel->setAlignment(Qt::AlignCenter);
        topBarLayout->addWidget(logoLabel, 0, Qt::AlignLeft);
        topBarLayout->addSpacing(3);

        auto titleSeparator = new QLabel(this);
        titleSeparator->setObjectName("TitleSeparator");
        titleSeparator->setPixmap(QIcon(":/icons/breadcrumb-chevron.svg").pixmap(8, 12));
        topBarLayout->addWidget(titleSeparator, 0, Qt::AlignLeft);

        // Current library
        m_libraryButton = new QToolButton(this);
        m_libraryButton->setObjectName("LibraryModeButton");
        m_libraryButton->setIcon(QIcon(":/icons/library.svg"));
        m_libraryButton->setIconSize(QSize(16, 16));
        m_libraryButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        m_libraryButton->setAutoRaise(true);
        topBarLayout->addWidget(m_libraryButton, 0, Qt::AlignLeft);
        QObject::connect(m_libraryButton,
                         &QToolButton::clicked,
                         m_context->uiState,
                         [state = m_context->uiState]() {
                                 state->setMode(NfUiMode::Library);
                         });
        topBarLayout->addStretch(1);

        // Settings
        auto m_settingsButton = new QToolButton(this);
        m_settingsButton->setObjectName("SettingsButton");
        m_settingsButton->setToolTip("Open Settings");
        m_settingsButton->setAccessibleName("Settings");
        m_settingsButton->setIcon(QIcon(":/icons/settings.svg"));
        m_settingsButton->setIconSize(QSize(18, 18));
        m_settingsButton->setFixedSize(30, 30);
        m_settingsButton->setCursor(Qt::PointingHandCursor);
        m_settingsButton->setAutoRaise(true);
        topBarLayout->addWidget(m_settingsButton, 0, Qt::AlignLeft);
        QObject::connect(m_settingsButton,
                         &QToolButton::clicked,
                         this,
                         [this]() {
                                 NfSettingsDialog dialog(this);
                                 dialog.exec();
                         });
        topBarLayout->addSpacing(8);

        // Browser view mode bar
        //m_browserViewModeBar = new NfBrowserViewModeBar(this);
        //topBarLayout->addWidget(m_browserViewModeBar, 0, Qt::AlignLeft);
        //topBarLayout->addStretch(1);

        QObject::connect(m_context->uiState,
                         &NfUiState::modeChanged,
                         this,
                         &NfTopBar::onModeChanged);

        auto libraryState = m_context->uiState->libraryModeState();
        QObject::connect(libraryState,
                         &NfUiLibraryModeState::currentLibraryChanged,
                         this,
                         &NfTopBar::updateLibraryName);
        updateLibraryName(libraryState->currentLibrary());

        onModeChanged(m_context->uiState->mode());
}

void NfTopBar::onModeChanged(NfUiMode mode)
{
        switch (mode) {
        case NfUiMode::Shoots:
                showShootsControls();
                break;

        case NfUiMode::Folders:
                showFolderControls();
                break;

        case NfUiMode::Library:
                showLibraryControls();
                break;
    }
}

void NfTopBar::showShootsControls()
{
        //auto state = m_context->uiState->shootsModeState()->browser();
        //showBrowserViewModeBar(state);
}

void NfTopBar::showFolderControls()
{
        auto state = m_context->uiState->folderModeState()->browser();
        showBrowserViewModeBar(state);
}

void NfTopBar::showLibraryControls()
{
        auto state = m_context->uiState->libraryModeState()->browser();
        showBrowserViewModeBar(state);
}

void NfTopBar::showBrowserViewModeBar(NfUiBrowserState* state)
{
        //m_browserViewModeBar->setState(state);
        //m_browserViewModeBar->show();
}

void NfTopBar::updateLibraryName(uint64_t libraryId)
{
        const auto library = m_context->library->getLibrary(libraryId);
        m_libraryButton->setText(library
                                 ? QString::fromStdString(library->name())
                                 : QString{});
}

} // namespace NfDesktop
