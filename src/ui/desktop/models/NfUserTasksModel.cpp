/**
 * File name: NfUserTasksModel.h
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

#include "NfUserTasksModel.h"

#include "NfUserTasksAdapter.h"
#include "core/NfImportFolderTaskData.h"
#include "core/utils/NfFileUtils.h"

#include <QString>

#include <variant>

using namespace NfCore;

namespace NfDesktop {

NfUserTasksModel::NfUserTasksModel(NfUi::NfUserTasksAdapter* adapter,
                                   QObject* parent)
        : QAbstractListModel(parent)
        , m_adapter{adapter}
{
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::taskAdded,
                         this, &NfUserTasksModel::onTaskAdded);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::taskUpdated,
                         this, &NfUserTasksModel::onTaskUpdated);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::taskRemoved,
                         this, &NfUserTasksModel::onTaskRemoved);
}

int NfUserTasksModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_tasks.size());
}

QVariant NfUserTasksModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid()
            || static_cast<std::size_t>(index.row()) >= m_tasks.size())
                return {};

        switch (role) {
        case Qt::DisplayRole:
                return tr("Import folder to library");
        case TaskDataRole: {
                const auto& item = m_tasks[static_cast<std::size_t>(index.row())];
                if (const auto* data =
                            std::get_if<NfImportFolderTaskData>(&item.data))
                        return QVariant::fromValue(*data);
                break;
        }
        }

        return {};
}

int NfUserTasksModel::rowByTaskId(NfTask::TaskId id) const
{
        for (std::size_t row = 0; row < m_tasks.size(); ++row) {
                if (m_tasks[row].id == id)
                        return static_cast<int>(row);
        }
        return -1;
}

void NfUserTasksModel::onTaskAdded(TaskId id, NfUserTaskData data)
{
        const auto row = rowByTaskId(id);
        if (row >= 0) {
                // Update existing item
                updateTakItem(row, std::move(data));
                return;
        }

        // Insert new item
        const auto newRow = static_cast<int>(m_tasks.size());
        beginInsertRows({}, newRow, newRow);
        m_tasks.push_back({id, std::move(data)});
        endInsertRows();

        rebuildIndexMap();
}

void NfUserTasksModel::onTaskUpdated(TaskId id, NfUserTaskData data)
{
        const auto row = rowByTaskId(id);
        if (row < 0)
                return;

        updateTakItem(row, std::move(data));
}

void NfUserTasksModel::onTaskRemoved(TaskId id)
{
        const auto row = rowByTaskId(id);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_tasks.erase(m_tasks.begin() + row);
        endRemoveRows();

        rebuildIndexMap();
}

void NfUserTasksModel::updateTakItem(int row, NfUserTaskData taskData)
{
        auto& taskItem = m_tasks[row];
        if (taskItem.data != taskData) {
                taskItem.data = std::move(taskData);
                const auto modelIndex = index(row);
                emit dataChanged(modelIndex, modelIndex);
        }
}

void NfUserTasksModel::rebuildIndexMap()
{
        m_taskIndexMap.clear();
        m_taskIndexMap.reserve(m_tasks.size());
        for (int row = 0; row < static_cast<int>(m_tasks.size()); row++)
                m_taskIndexMap[m_tasks[row].id] = row;
}

} // namespace NfDesktop
