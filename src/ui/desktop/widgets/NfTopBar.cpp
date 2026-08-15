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
#include "NfUiShootsModeState.h"
#include "NfUiFolderModeState.h"
#include "NfUiLibraryModeState.h"
#include "NfUiBrowserState.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>

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

        // Logo
        QIcon logoIcon(":/logo.svg");
	auto logoLabel = new QLabel(this);
	logoLabel->setAttribute(Qt::WA_TranslucentBackground);
	logoLabel->setPixmap(logoIcon.pixmap(24, 24));
        logoLabel->setFixedSize(24, 24);
        topBarLayout->addWidget(logoLabel, 0, Qt::AlignLeft);
        topBarLayout->addStretch(1);

        // Browser view mode bar
        //m_browserViewModeBar = new NfBrowserViewModeBar(this);
        //topBarLayout->addWidget(m_browserViewModeBar, 0, Qt::AlignLeft);
        //topBarLayout->addStretch(1);

        QObject::connect(m_context->uiState,
                         &NfUiState::modeChanged,
                         this,
                         &NfTopBar::onModeChanged);

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

} // namespace NfDesktop
