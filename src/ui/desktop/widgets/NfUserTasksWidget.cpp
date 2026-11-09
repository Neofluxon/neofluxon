/**
 * File name: NfUserTasksWidget.cpp
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

#include "NfUserTasksWidget.h"

#include "NfUserTaskDelegate.h"
#include "NfUserTasksAdapter.h"
#include "NfUserTasksModel.h"
#include "NfUserTasksView.h"
#include "core/NfImportFolderTaskData.h"
#include "core/NfUserTaskManager.h"

#include <QAbstractItemModel>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>

namespace NfDesktop {

NfUserTasksWidget::NfUserTasksWidget(NfCore::NfUserTaskManager* taskManager,
                                     QWidget* parent)
        : QWidget(parent)
        , m_taskButton{new QToolButton(this)}
        , m_taskPopup{new QFrame(window(), Qt::Popup | Qt::FramelessWindowHint)}
        , m_countLabel{}
        , m_taskModel{}
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_taskButton);

        m_taskButton->setObjectName(QStringLiteral("TaskStatusButton"));
        m_taskButton->setIconSize(QSize(14, 14));
        m_taskButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        m_taskButton->setText(tr("Activity"));
        m_taskButton->setToolTip(tr("Show activity"));
        m_taskButton->setAccessibleName(tr("Show activity"));
        m_taskButton->setAutoRaise(true);

        m_taskPopup->setObjectName(QStringLiteral("TaskPopup"));
        m_taskPopup->setAttribute(Qt::WA_StyledBackground, true);
        m_taskPopup->setFixedWidth(400);

        auto* popupLayout = new QVBoxLayout(m_taskPopup);
        popupLayout->setContentsMargins(16, 10, 12, 16);
        popupLayout->setSpacing(8);

        auto* headerLayout = new QHBoxLayout;
        auto* headerLabel = new QLabel(tr("Activity"), m_taskPopup);
        headerLabel->setObjectName(QStringLiteral("TaskPopupTitle"));
        m_countLabel = new QLabel(m_taskPopup);
        m_countLabel->setObjectName(QStringLiteral("TaskPopupCount"));
        headerLayout->addWidget(headerLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(m_countLabel);
        popupLayout->addLayout(headerLayout);

        auto* taskAdapter = new NfUi::NfUserTasksAdapter(taskManager, this);
        m_taskModel = new NfUserTasksModel(taskAdapter, this);

        QObject::connect(m_taskModel, &QAbstractItemModel::rowsInserted,
                         this, &NfUserTasksWidget::updateTaskStatus);
        QObject::connect(m_taskModel, &QAbstractItemModel::rowsRemoved,
                         this, &NfUserTasksWidget::updateTaskStatus);
        QObject::connect(m_taskModel, &QAbstractItemModel::dataChanged,
                         this, &NfUserTasksWidget::updateTaskStatus);

        updateTaskStatus();

        popupLayout->addWidget(new NfUserTasksView(m_taskModel, m_taskPopup));

        QObject::connect(m_taskButton, &QToolButton::clicked,
                         this, &NfUserTasksWidget::toggleTaskPopup);
}

void NfUserTasksWidget::updateTaskStatus()
{
        const auto taskCount = m_taskModel->rowCount();
        m_countLabel->setText(tr("%n activities", nullptr, taskCount));
        setVisible(taskCount > 0);
        m_taskButton->setText(tr("Activity (%1)").arg(taskCount));
}

void NfUserTasksWidget::toggleTaskPopup()
{
        if (m_taskPopup->isVisible()) {
                m_taskPopup->hide();
                return;
        }

        m_taskPopup->adjustSize();
        const auto buttonTopLeft = m_taskButton->mapToGlobal(QPoint(0, 0));
        const auto windowBottomRight =
                window()->mapToGlobal(window()->rect().bottomRight());
        constexpr int popupEdgeSpacing = 12;
        const auto popupX = std::min(
                buttonTopLeft.x() + m_taskButton->width() - m_taskPopup->width(),
                windowBottomRight.x() - m_taskPopup->width() - popupEdgeSpacing);
        m_taskPopup->move(popupX,
                          buttonTopLeft.y() - m_taskPopup->height()
                                  - popupEdgeSpacing);
        m_taskPopup->show();
}

} // namespace NfDesktop
