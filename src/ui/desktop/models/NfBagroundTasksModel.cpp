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
#include "NfBackgroundTasksAdapter.h"

#include <algorithm>
#include <iterator>

namespace NfDesktop {

NfBagroundTasksModel::NfBagroundTasksModel(NfUi::NfBackgroundTasksAdapter* adapter,
                                           QObject* parent)
        : QAbstractListModel(parent)
        , m_adapter{adapter}
        , m_tasks{adapter->tasks()}
{
        QObject::connect(m_adapter, &NfUi::NfBackgroundTasksAdapter::taskAdded,
                         this, &NfBagroundTasksModel::onTaskAdded);
        QObject::connect(m_adapter, &NfUi::NfBackgroundTasksAdapter::taskUpdated,
                         this, &NfBagroundTasksModel::onTaskUpdated);
        QObject::connect(m_adapter, &NfUi::NfBackgroundTasksAdapter::taskRemoved,
                         this, &NfBagroundTasksModel::onTaskRemoved);
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
        case TaskIdRole:
                return task.id;
        default:
                return {};
        }
}

int NfBagroundTasksModel::count(NfUi::NfBackgroundTask::State state) const
{
        int taskCount = 0;
        for (const auto& task : m_tasks) {
                if (task.state == state)
                        ++taskCount;
        }
        return taskCount;
}

void NfBagroundTasksModel::requestAction(const QModelIndex& index)
{
        if (!index.isValid() || index.row() < 0
            || static_cast<std::size_t>(index.row()) >= m_tasks.size())
                return;

        const auto& task = m_tasks.at(static_cast<std::size_t>(index.row()));
        switch (task.state) {
        case NfUi::NfBackgroundTask::State::Running:
                m_adapter->requestCancelTask(task.id);
                break;
        case NfUi::NfBackgroundTask::State::Paused:
                m_adapter->requestResumeTask(task.id);
                break;
        case NfUi::NfBackgroundTask::State::Stopped:
                m_adapter->requestRestartTask(task.id);
                break;
        }
}

void NfBagroundTasksModel::onTaskAdded(const NfUi::NfBackgroundTask& task)
{
        if (rowForTaskId(task.id) >= 0)
                return;

        const auto row = static_cast<int>(m_tasks.size());
        beginInsertRows({}, row, row);
        m_tasks.push_back(task);
        endInsertRows();
}

void NfBagroundTasksModel::onTaskUpdated(const NfUi::NfBackgroundTask& task)
{
        const auto row = rowForTaskId(task.id);
        if (row < 0)
                return;

        m_tasks[static_cast<std::size_t>(row)] = task;
        const auto taskIndex = index(row);
        emit dataChanged(taskIndex, taskIndex);
}

void NfBagroundTasksModel::onTaskRemoved(const QString& taskId)
{
        const auto row = rowForTaskId(taskId);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_tasks.erase(m_tasks.begin() + row);
        endRemoveRows();
}

int NfBagroundTasksModel::rowForTaskId(const QString& taskId) const
{
        const auto it = std::find_if(m_tasks.begin(), m_tasks.end(),
                                     [&taskId](const auto& task) {
                                             return task.id == taskId;
                                     });
        return it == m_tasks.end()
                ? -1
                : static_cast<int>(std::distance(m_tasks.begin(), it));
}

} // namespace NfDesktop
