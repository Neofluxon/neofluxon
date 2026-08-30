/**
 * File name: NfRightPanel.cpp
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

#include "NfRightPanel.h"
#include "NfContext.h"
#include "NfUiState.h"
#include "NfImageMetadataPanel.h"
#include "core/NfLogger.h"

#include <QVBoxLayout>
#include <QStackedWidget>

using namespace NfUi;

namespace NfDesktop {

NfRightPanel::NfRightPanel(NfContext *ctx, QWidget *parent)
        : NfPanel(parent, NfPanel::PanelPosition::AlignRight)
        , m_context{ctx}
        , m_imageMetadata{new NfImageMetadataPanel(this)}
        , m_stack{new QStackedWidget(this)}
{
        auto panelLayout = new QVBoxLayout(this);
        m_stack->addWidget(m_imageMetadata);

        panelLayout->addWidget(m_stack);

        setLayout(panelLayout);
}

} // namespace NfDesktop
