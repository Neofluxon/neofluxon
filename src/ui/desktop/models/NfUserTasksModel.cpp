/**
 * File name: NfUserTasksModel.cpp
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
 */

#include "NfUserTasksModel.h"

#include "NfUserTasksAdapter.h"
#include "utils/NfFileUtils.h"

#include <QString>
#include <any>

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
        if (!index.isValid() || index.row() < 0
            || static_cast<std::size_t>(index.row()) >= m_tasks.size())
                return {};

        const auto& item = m_tasks.at(static_cast<std::size_t>(index.row()));
        const auto taskType = m_taskTypes.find(item.id);
        if (taskType == m_taskTypes.end())
                return {};

        switch (role) {
        case Qt::DisplayRole:
                switch (taskType->second) {
                case NfCore::NfUserTaskType::ImportFolder:
                        return tr("Import folder");
                }
                return {};
        case TaskIdRole:
                return QVariant::fromValue<qulonglong>(item.id);
        case TaskTypeRole:
                return static_cast<int>(taskType->second);
        default:
                break;
        }

        switch (taskType->second) {
        case NfCore::NfUserTaskType::ImportFolder: {
                const auto* data =
                        std::any_cast<NfCore::NfImportFolderTaskData>(&item.data.taskData);
                if (!data)
                        return {};
                switch (role) {
                case SourcePathRole:
                        return QString::fromStdString(
                                NfCore::NfFileUtils::pathToUtf8(data->sourceFolder));
                case StatusRole:
                        return static_cast<int>(data->status);
                case DiscoveredPhotosRole:
                        return QVariant::fromValue<qulonglong>(data->discoveredPhotos);
                case ProcessedPhotosRole:
                        return QVariant::fromValue<qulonglong>(data->processedPhotos);
                case ImportedPhotosRole:
                        return QVariant::fromValue<qulonglong>(data->importedPhotos);
                case SkippedPhotosRole:
                        return QVariant::fromValue<qulonglong>(data->skippedPhotos);
                case FailedPhotosRole:
                        return QVariant::fromValue<qulonglong>(data->failedPhotos);
                default:
                        return {};
                }
        }
        }
        return {};
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

void NfUserTasksModel::onTaskAdded(
        qulonglong id, NfCore::NfUserTaskData data)
{
        const auto existingRow = rowForTaskId(id);
        if (existingRow >= 0) {
                onTaskUpdated(id, std::move(data));
                return;
        }

        const auto row = static_cast<int>(m_tasks.size());
        beginInsertRows({}, row, row);
        m_taskTypes[id] = data.type;
        m_tasks.push_back({id, std::move(data)});
        endInsertRows();
}

void NfUserTasksModel::onTaskUpdated(
        qulonglong id, NfCore::NfUserTaskData data)
{
        const auto row = rowForTaskId(id);
        if (row < 0)
                return;

        m_taskTypes[id] = data.type;
        m_tasks[static_cast<std::size_t>(row)].data = std::move(data);
        const auto modelIndex = index(row);
        emit dataChanged(modelIndex, modelIndex);
}

void NfUserTasksModel::onTaskRemoved(qulonglong id)
{
        const auto row = rowForTaskId(id);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_tasks.erase(m_tasks.begin() + row);
        m_taskTypes.erase(id);
        endRemoveRows();
}

} // namespace NfDesktop
