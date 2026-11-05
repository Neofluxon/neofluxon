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
#include "NfUserTasksAdapter.h"
#include "NfUserTasksModel.h"
#include "NfUserTasksView.h"
#include "core/NfApplicationPaths.h"
#include "core/NeofluxonCore.h"

#include <QFrame>
#include <QLabel>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QSplitter>
#include <QStatusBar>
#include <QToolButton>

#include <algorithm>

using namespace NfUi;

namespace NfDesktop {

NfMainWindow::NfMainWindow(NfContext *ctx)
        : QMainWindow()
        , m_context{ctx}
        , m_splitter{new QSplitter(Qt::Horizontal)}
{
        setWindowTitle("Neofluxon");
        statusBar()->setObjectName(QStringLiteral("NfStatusBar"));
        statusBar()->setSizeGripEnabled(false);

        auto* taskButton = new QToolButton(statusBar());
        taskButton->setObjectName(QStringLiteral("TaskStatusButton"));
        taskButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
        taskButton->setIconSize(QSize(14, 14));
        taskButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        taskButton->setText(tr("User tasks"));
        taskButton->setToolTip(tr("Show user tasks"));
        taskButton->setAccessibleName(tr("Show user tasks"));
        taskButton->setAutoRaise(true);
        statusBar()->addPermanentWidget(taskButton);

        auto* taskPopup = new QFrame(this, Qt::Popup | Qt::FramelessWindowHint);
        taskPopup->setObjectName(QStringLiteral("TaskPopup"));
        taskPopup->setAttribute(Qt::WA_StyledBackground, true);
        taskPopup->setFixedWidth(400);

        auto* popupLayout = new QVBoxLayout(taskPopup);
        popupLayout->setContentsMargins(16, 10, 12, 16);
        popupLayout->setSpacing(8);

        auto* headerLayout = new QHBoxLayout;
        auto* headerLabel = new QLabel(tr("User tasks"), taskPopup);
        headerLabel->setObjectName(QStringLiteral("TaskPopupTitle"));
        auto* countLabel = new QLabel(taskPopup);
        countLabel->setObjectName(QStringLiteral("TaskPopupCount"));
        headerLayout->addWidget(headerLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(countLabel);
        popupLayout->addLayout(headerLayout);

        auto* taskAdapter = new NfUi::NfUserTasksAdapter(
                m_context->core()->userTaskManager(), this);
        auto* taskModel = new NfUserTasksModel(taskAdapter, this);
        const auto updateTaskStatus = [taskButton, countLabel, taskModel]() {
                using Status = NfCore::NfImportFolderTaskData::Status;
                const auto active = taskModel->count(Status::Scanning)
                        + taskModel->count(Status::Importing);
                const auto finished = taskModel->count(Status::Completed)
                        + taskModel->count(Status::Failed)
                        + taskModel->count(Status::Cancelled);
                countLabel->setText(
                        QObject::tr("%1 active · %2 finished")
                                .arg(active).arg(finished));
                taskButton->setVisible(taskModel->rowCount() > 0);
                taskButton->setText(QObject::tr("User tasks (%1)")
                                             .arg(taskModel->rowCount()));
        };
        QObject::connect(taskModel, &QAbstractItemModel::rowsInserted,
                         this, updateTaskStatus);
        QObject::connect(taskModel, &QAbstractItemModel::rowsRemoved,
                         this, updateTaskStatus);
        QObject::connect(taskModel, &QAbstractItemModel::dataChanged,
                         this, updateTaskStatus);
        updateTaskStatus();
        auto* taskView = new NfUserTasksView(taskModel, taskPopup);
        popupLayout->addWidget(taskView);

        QObject::connect(taskButton, &QToolButton::clicked, this,
                         [this, taskButton, taskPopup]() {
                                 if (taskPopup->isVisible()) {
                                         taskPopup->hide();
                                         return;
                                 }

                                 taskPopup->adjustSize();
                                 const auto buttonTopLeft =
                                         taskButton->mapToGlobal(QPoint(0, 0));
                                 const auto windowBottomRight =
                                         mapToGlobal(rect().bottomRight());
                                 constexpr int popupEdgeSpacing = 12;
                                 const auto popupX = std::min(
                                         buttonTopLeft.x() + taskButton->width()
                                                 - taskPopup->width(),
                                         windowBottomRight.x() - taskPopup->width()
                                                 - popupEdgeSpacing);
                                 taskPopup->move(
                                         popupX,
                                         buttonTopLeft.y() - taskPopup->height()
                                                 - popupEdgeSpacing);
                                 taskPopup->show();
                         });

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
