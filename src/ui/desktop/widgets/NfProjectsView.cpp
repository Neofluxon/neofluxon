/**
 * File name: NfProjectsView.cpp
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

#include "NfProjectsView.h"
#include "NfDesktopContext.h"
#include "NfUiState.h"
#include "NfUiProjectsModeState.h"
#include "NfProjectsModel.h"
#include "NfProjectsListView.h"

#include <QVBoxLayout>

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfProjectsView::NfProjectsView(const NfDesktopContext &ctx,
                             QWidget* parent)
        : QWidget(parent)
        , m_state{ctx.uiState()->projectsModeState()}
        , m_model{new NfProjectsModel(ctx, this)}
        , m_mainLayout{nullptr}
        , m_projectsListView{nullptr}
{
        setAttribute(Qt::WA_StyledBackground, true);

        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(6);

        m_projectsListView = new NfProjectsListView(ctx, m_model, this);
        m_mainLayout->addWidget(m_projectsListView);
}

} // namespace NfDesktop
