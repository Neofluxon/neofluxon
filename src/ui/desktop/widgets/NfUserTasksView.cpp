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

#include <algorithm>

using namespace NfCore;

namespace NfDesktop {

// TODO: Implement failed and paused tasks
//       to be retained and managed buy the user

NfUserTasksView::NfUserTasksView(NfUserTasksModel* model,
                                 QWidget* parent)
        : QListView(parent)
{
        setObjectName(QStringLiteral("NfUserTasksView"));
        setModel(model);
        setItemDelegate(new NfUserTaskDelegate(this));
        setFrameShape(QFrame::NoFrame);
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setSpacing(8);
        setUniformItemSizes(true);

        QObject::connect(model, &QAbstractItemModel::rowsInserted,
                         this, [this]() { updateHeight(); updateActivity(); });
        QObject::connect(model, &QAbstractItemModel::rowsRemoved,
                         this, [this]() { updateHeight(); updateActivity(); });
        QObject::connect(model, &QAbstractItemModel::dataChanged,
                         this, [this]() { updateActivity(); });

        // We need this to make the spinner in the task delegate
        // to move when there are tasks with status importing
        QObject::connect(&m_activityTimer, &QTimer::timeout,
                         this, [this]() { viewport()->update(); });
        m_activityTimer.setInterval(80);

        updateHeight();
        updateActivity();
}

void NfUserTasksView::updateHeight()
{
        // We need to update the height if tasks added/removed.
        const auto visibleRows = std::min(model()->rowCount(), m_maxVisibleRows);
        const auto rowHeight = itemDelegate()->sizeHint(QStyleOptionViewItem{},
                                                        QModelIndex{}).height();
        setFixedHeight(visibleRows * rowHeight
                       + std::max(0, visibleRows - 1) * spacing());
}

void NfUserTasksView::setMaxVisibleRows(int rows)
{
        m_maxVisibleRows = std::max(0, rows);
        updateHeight();
}

void NfUserTasksView::updateActivity()
{
        const auto* taskModel = qobject_cast<NfUserTasksModel*>(model());
        const bool active = taskModel && taskModel->rowCount() > 0;
        if (active && !m_activityTimer.isActive())
                m_activityTimer.start();
        else if (!active && m_activityTimer.isActive())
                m_activityTimer.stop();
}

} // namespace NfDesktop
