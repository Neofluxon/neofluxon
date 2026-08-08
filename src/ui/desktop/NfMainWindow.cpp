/**
 * File name: NfMainWindow.cpp
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

#include "NfMainWindow.h"
#include "NfContext.h"
#include "NfTopBar.h"
#include "NfPanel.h"
#include "NfLeftPanel.h"
#include "NfStyledWidget.h"
#include "NfCentralWidget.h"
#include "NfLibraryAdapter.h"
#include "core/NeofluxonCore.h"

#include <QFrame>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace NfUi;

namespace NfDesktop {

NfMainWindow::NfMainWindow(NfContext *ctx)
        : QMainWindow()
        , m_context{ctx}
{
        setWindowTitle("Neofluxon");

        m_context->library = new NfLibraryAdapter(m_context->core()->libraryManger(), this);

        resize(1400, 800);
        setGeometry(200, 200, 1400, 800);

        auto container = new QWidget(this);
        container->setObjectName("container");

        auto *vLayout = new QVBoxLayout(container);
        vLayout->setContentsMargins(0,0,0,0);
        vLayout->setSpacing(0);

        auto topBar = new NfTopBar();
        vLayout->addWidget(topBar);

        auto *hLayout = new QHBoxLayout;
        hLayout->setContentsMargins(0,0,0,0);
        hLayout->setSpacing(0);

        auto leftPanel = new NfLeftPanel(m_context, this);
        hLayout->addWidget(leftPanel);

        hLayout->addWidget(new NfCentralWidget(m_context, this));

        auto rightPanel = new NfPanel(nullptr, NfPanel::PanelPosition::AlignRight);
        hLayout->addWidget(rightPanel);

        QWidget *contentWrapper = new QWidget;
        contentWrapper->setAttribute(Qt::WA_TranslucentBackground);
        contentWrapper->setLayout(hLayout);
        vLayout->addWidget(contentWrapper);

        leftPanel->raise();
        rightPanel->raise();
        topBar->raise();

        setCentralWidget(container);
}

NfMainWindow::~NfMainWindow()
{
}

} // namespace NfDesktop
