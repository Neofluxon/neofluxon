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
#include "NfLeftPanel.h"
#include "NfRightPanel.h"
#include "NfStyledWidget.h"
#include "NfCentralWidget.h"
#include "NfLibraryAdapter.h"
#include "NfMetadataProvider.h"
#include "NfUserTasksWidget.h"
#include "core/NfApplicationPaths.h"
#include "core/NeofluxonCore.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStatusBar>

using namespace NfUi;

namespace NfDesktop {

NfMainWindow::NfMainWindow(NfContext *ctx)
        : QMainWindow()
        , m_context{ctx}
        , m_splitter{new QSplitter(Qt::Horizontal)}
{
        setWindowTitle(QStringLiteral("%1").arg(QGuiApplication::applicationDisplayName()));
        statusBar()->setObjectName(QStringLiteral("NfStatusBar"));
        statusBar()->setSizeGripEnabled(false);
        statusBar()->addPermanentWidget(new NfUserTasksWidget(m_context->core()->userTaskManager(),
                                                              this));

        m_context->library = new NfLibraryAdapter(m_context->core()->libraryManger(), this);
        m_context->metadataProvider = new NfMetadataProvider(m_context->core(), this);

        resize(1400, 800);
        setGeometry(200, 200, 1400, 800);

        auto container = new QWidget(this);
        container->setObjectName("container");

        auto *vLayout = new QVBoxLayout(container);
        vLayout->setContentsMargins(0,0,0,0);
        vLayout->setSpacing(0);

        auto topBar = new NfTopBar(m_context);
        vLayout->addWidget(topBar);

        auto *hLayout = new QHBoxLayout;
        hLayout->setContentsMargins(0,0,0,0);
        hLayout->setSpacing(0);

        m_splitter->setObjectName(QStringLiteral("MainSplitter"));
        m_splitter->setChildrenCollapsible(false);

        // Resizable, has minimum width, and is collapsible
        auto leftPanel = new NfLeftPanel(m_context, this);
        leftPanel->setMinimumWidth(220);
        m_splitter->addWidget(leftPanel);
        m_splitter->addWidget(new NfCentralWidget(m_context, this));

        // Resizable, has minimum and minmum width, and is collapsible
        auto rightPanel = new NfRightPanel(m_context, this);
        rightPanel->setMinimumWidth(240);
        rightPanel->setMaximumWidth(420);
        m_splitter->addWidget(rightPanel);

        // Set both panels are collapsable
        m_splitter->setCollapsible(0, true);
        m_splitter->setCollapsible(2, true);

        m_splitter->setStretchFactor(0, 0);
        m_splitter->setStretchFactor(1, 1);
        m_splitter->setStretchFactor(2, 0);

        // Default widths: left panel, central widget, left panel
        m_splitter->setSizes({300, 800, 300});

        hLayout->addWidget(m_splitter, 1);

        QWidget *contentWrapper = new QWidget;
        contentWrapper->setAttribute(Qt::WA_TranslucentBackground);
        contentWrapper->setLayout(hLayout);
        vLayout->addWidget(contentWrapper);

        topBar->raise();

        setCentralWidget(container);
}

NfMainWindow::~NfMainWindow()
{
}

} // namespace NfDesktop
