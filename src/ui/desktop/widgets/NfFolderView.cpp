/**
 * File name: NfFolderView.cpp
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

#include "NfFolderView.h"
#include "NfUiState.h"
#include "NfUiFolderModeState.h"
#include "NfFolderModel.h"
#include "NfBrowserModel.h"
#include "NfBrowserViewModeBar.h"
//#include "NfBreadcrumbView.h"
#include "NfBrowserView.h"
#include "core/NfPhotoSource.h"

#include <QVBoxLayout>

using namespace NfUi;

namespace NfDesktop {

NfFolderView::NfFolderView(NfFolderContext ctx,
                           QWidget* parent)
        : QWidget(parent)
        , m_context{ctx}
        , m_state{m_context.uiState()->folderModeState()}
        , m_model{new NfFolderModel(std::move(ctx), this)}
        , m_mainLayout{nullptr}
        , m_browserView{nullptr}
{
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(5);

        auto browserViewModeBar = new NfBrowserViewModeBar(this);
        browserViewModeBar->setState(m_state->browser());
        browserViewModeBar->setFixedSize(450, 32);
        m_mainLayout->addWidget(browserViewModeBar);
        //topBarLayout->addWidget(m_browserViewModeBar, 0, Qt::AlignLeft);
        //topBarLayout->addStretch(1);


        //auto breadcrumbBar = new NfBreadcrumbView(this);
        //breadcrumbBar->setObjectName("BrowserBreadcrumb");
        //breadcrumbBar->setModel(m_model->breadcrump());
        //m_mainLayout->addWidget(breadcrumbBar);

        m_browserView = new NfBrowserView(m_state->browser(),
                                          m_model->browser(),
                                          this);

        QObject::connect(m_state,
                         &NfUiFolderModeState::pathChanged,
                         m_model,
                         &NfFolderModel::setPath);

        m_mainLayout->addWidget(m_browserView);
}

} // namespace NfDesktop
