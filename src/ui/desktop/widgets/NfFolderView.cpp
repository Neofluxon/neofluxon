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
#include "NfUiBrowserState.h"
#include "NfUiFolderModeState.h"
#include "NfFolderModel.h"
#include "NfBrowserModel.h"
#include "NfBrowserTopBar.h"
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
        setAttribute(Qt::WA_StyledBackground, true);
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(6);

        auto browserTopBar = new NfBrowserTopBar(m_context.uiState(), this);
        m_mainLayout->addWidget(browserTopBar);

        m_browserView = new NfBrowserView(m_state->browser(),
                                          m_model->browser(),
                                          this);

        QObject::connect(m_state,
                         &NfUiFolderModeState::pathChanged,
                         this,
                         [this](const std::filesystem::path& path) {
                                 m_model->setPath(path,
                                                  m_state->browser()->scanRecursively());
                         });
        QObject::connect(m_state->browser(),
                         &NfUiBrowserState::scanRecursivelyChanged,
                         this,
                         [this](bool recursive) {
                                 if (!m_state->path().empty())
                                         m_model->setPath(m_state->path(), recursive);
                         });

        if (!m_state->path().empty())
                m_model->setPath(m_state->path(),
                                 m_state->browser()->scanRecursively());

        m_mainLayout->addWidget(m_browserView);
}

} // namespace NfDesktop
