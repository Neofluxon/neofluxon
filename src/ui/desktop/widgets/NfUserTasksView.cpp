/**
 * File name: NfUserTasksView.cpp
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
 * This program is distributed in the hope that it will be useful;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "NfUserTasksView.h"
#include "NfUserTaskDelegate.h"
#include "NfUserTasksModel.h"

#include <QAbstractItemModel>

namespace NfDesktop {

NfUserTasksView::NfUserTasksView(NfUserTasksModel* model,
                                         QWidget* parent)
        : QListView(parent)
{
        constexpr int rowSpacing = 8;

        setObjectName(QStringLiteral("NfUserTasksView"));
        setModel(model);
        setItemDelegate(new NfUserTaskDelegate(this));
        setFrameShape(QFrame::NoFrame);
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setSpacing(rowSpacing);
        setUniformItemSizes(true);

        QObject::connect(model, &QAbstractItemModel::rowsInserted,
                         this, [this]() { updateHeight(); updateActivity(); });
        QObject::connect(model, &QAbstractItemModel::rowsRemoved,
                         this, [this]() { updateHeight(); updateActivity(); });
        QObject::connect(model, &QAbstractItemModel::dataChanged,
                         this, [this]() { updateActivity(); });
        QObject::connect(&m_activityTimer, &QTimer::timeout,
                         this, [this]() { viewport()->update(); });
        m_activityTimer.setInterval(80);
        updateHeight();
        updateActivity();
}

void NfUserTasksView::updateHeight()
{
        constexpr int rowHeight = 116;
        constexpr int rowSpacing = 8;
        constexpr int maxVisibleRows = 5;
        const auto visibleRows = qMin(model()->rowCount(), maxVisibleRows);
        setFixedHeight(visibleRows * rowHeight
                       + qMax(0, visibleRows - 1) * rowSpacing);
}

void NfUserTasksView::updateActivity()
{
        const auto* taskModel = qobject_cast<NfUserTasksModel*>(model());
        const bool active = taskModel
                && (taskModel->count(NfCore::NfImportFolderTaskData::Status::Scanning)
                    + taskModel->count(
                            NfCore::NfImportFolderTaskData::Status::Importing) > 0);
        if (active && !m_activityTimer.isActive())
                m_activityTimer.start();
        else if (!active && m_activityTimer.isActive())
                m_activityTimer.stop();
}

} // namespace NfDesktop
