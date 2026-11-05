/**
 * File name: NfBagroundTasksModel.cpp
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

#include "NfBagroundTasksModel.h"

namespace NfDesktop {

NfBagroundTasksModel::NfBagroundTasksModel(QObject* parent)
        : QAbstractListModel(parent)
        , m_tasks{
                {tr("Import folder \"/home/user/folder1\" to library"),
                 QStringLiteral("/home/user/folder1"),
                 NfBagroundTask::State::Running,
                 42,
                 tr("1,240 / 2,950 photos"),
                 tr("Cancel")}}
{
}

int NfBagroundTasksModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_tasks.size());
}

QVariant NfBagroundTasksModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() < 0
            || static_cast<std::size_t>(index.row()) >= m_tasks.size())
                return {};

        const auto& task = m_tasks.at(static_cast<std::size_t>(index.row()));
        switch (role) {
        case Qt::DisplayRole:
        case OperationRole:
                return task.operation;
        case SourcePathRole:
                return task.sourcePath;
        case StateRole:
                return static_cast<int>(task.state);
        case ProgressRole:
                return task.progress;
        case ProgressTextRole:
                return task.progressText;
        case ActionTextRole:
                return task.actionText;
        default:
                return {};
        }
}

int NfBagroundTasksModel::count(NfBagroundTask::State state) const
{
        int taskCount = 0;
        for (const auto& task : m_tasks) {
                if (task.state == state)
                        ++taskCount;
        }
        return taskCount;
}

} // namespace NfDesktop
