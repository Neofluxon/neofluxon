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
#include "utils/NfFileUtils.h"
#include "core/NfImportFolderTaskData.h"

#include <QString>

#include <variant>

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
        case TaskDataRole:
                const auto& item = m_tasks[static_cast<std::size_t>(index.row())];
                if (const auto *data = std::get_if<NfImportFolderData>(&item.data()))
                        return data(item.data, role);

        return {};
}

QVariant NfUserTasksModel::data(const NfImportFolderData& data, int role) const
{
        switch (role) {
        case Qt::DisplayRole:
                return tr("Import folder to library");
        case DataRole::SourcePathRole:
                return QString::fromStdString(
                                              NfFileUtils::pathToUtf8(data->sourceFolder));
        case DataRole::DiscoveredPhotosRole:
                return QVariant::fromValue<qulonglong>(data->discoveredPhotos);
        case DataRole::ImportedPhotosRole:
                return QVariant::fromValue<qulonglong>(data->importedPhotos);
        case DataRole::FailedPhotosRole:
                return QVariant::fromValue<qulonglong>(data->failedPhotos);
        case DataRole::StatusRole:
                return static_cast<int>(data->status);
        default:
                return {};
        }
}

int NfUserTasksModel::count(NfCore::NfImportFolderTaskData::Status status) const
{
        int result = 0;
        for (const auto& task : m_tasks) {
                const auto type = m_taskTypes.find(task.id);
                if (type == m_taskTypes.end()
                    || type->second != NfCore::NfUserTaskType::ImportFolder)
                        continue;
                const auto* data =
                        std::any_cast<NfCore::NfImportFolderTaskData>(&task.data.taskData);
                if (data && data->status == status)
                        ++result;
        }
        return result;
}

int NfUserTasksModel::rowForTaskId(NfCore::NfTask::TaskId id) const
{
        for (std::size_t row = 0; row < m_tasks.size(); ++row) {
                if (m_tasks[row].id == id)
                        return static_cast<int>(row);
        }
        return -1;
}

void NfUserTasksModel::onTaskAdded(TaskId id, NfUserTaskData data)
{
        const auto row = getRowByTaskId(id);
        if (row >= 0) {
                // Update existing item
                updateTask(id, std::move(data));
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
        const auto row = getRowByTaskId(id);
        if (row < 0)
                return;

        updateTakItem(row, std::move(data));
}

void NfUserTasksModel::onTaskRemoved(qulonglong id)
{
        const auto row = getRowByTaskId(id);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_tasks.erase(m_tasks.begin() + row);
        endRemoveRows();

        rebuildIndexMap();
}

NfUserTasksModel::updateTakItem(int row, NfUserTaskData data)
{
        auto &taskItem = m_tasks[row];
        if (taskItem.data != data) {
                taskItem.data = std::move(data);
                const auto modelIndex = index(row);
                emit dataChanged(modelIndex, modelIndex);
        }
}

void NfUserTasksModel::rebuildIndexMap()
{
        m_indexMap.clear();
        m_indexMap.reserve(m_tasks.size());
        for (int row = 0; row < static_cast<int>(m_tasks.size()); row++)
                m_indexMap[m_tasks[row].id] = row;
}

} // namespace NfDesktop
