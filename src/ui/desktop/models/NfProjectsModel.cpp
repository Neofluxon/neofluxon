/**
 * File name: NfProjectsModel.h
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

#include "NfProjectsModel.h"

#include "NfProjectsAdapter.h"

using namespace NfCore;

namespace NfDesktop {

NfProjectsModel::NfProjectsModel(NfUi::NfProjectsdapter* adapter,
                                 QObject* parent)
        : QAbstractListModel(parent)
        , m_adapter{adapter}
{
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectAdded,
                         this, &NfProjectsModel::onProjectAdded);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectUpdated,
                         this, &NfProjectsModel::onProjectUpdated);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectRemoved,
                         this, &NfProjectsModel::onProjectRemoved);
}

int NfProjectsModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_projects.size());
}

QVariant NfProjectsModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid()
            || static_cast<std::size_t>(index.row()) >= m_projects.size())
                return {};

        switch (role) {
        case Qt::DisplayRole:
                return {};
        case ProjectDataRole:
                return QVariant::fromValue(m_projects[static_cast<std::size_t>(index.row())]);
        }

        return {};
}

int NfProjectsModel::rowByTaskId(NfTask::TaskId id) const
{
        for (std::size_t row = 0; row < m_projects.size(); ++row) {
                if (m_projects[row].id == id)
                        return static_cast<int>(row);
        }
        return -1;
}

void NfProjectsModel::onTaskAdded(TaskId id, NfUserTaskData data)
{
        const auto row = rowByTaskId(id);
        if (row >= 0) {
                // Update existing item
                updateTakItem(row, std::move(data));
                return;
        }

        // Insert new item
        const auto newRow = static_cast<int>(m_projects.size());
        beginInsertRows({}, newRow, newRow);
        m_projects.push_back(std::move(data));
        endInsertRows();

        rebuildIndexMap();
}

void NfProjectsModel::onTaskUpdated(NfProjectTaskData data)
{
        const auto& project = prorojectById(data.id);
        if (row < 0)
                return;

        updateProject(row, std::move(data));
}

void NfProjectsModel::onTaskRemoved(TaskId id)
{
        const auto row = rowByTaskId(id);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_projects.erase(m_projects.begin() + row);
        endRemoveRows();

        rebuildIndexMap();
}

void NfProjectsModel::updateTakItem(int row, NfUserTaskData taskData)
{
        auto& taskItem = m_projects[row];
        if (taskItem.data != taskData) {
                taskItem.data = std::move(taskData);
                const auto modelIndex = index(row);
                emit dataChanged(modelIndex, modelIndex);
        }
}

void NfProjectsModel::rebuildIndexMap()
{
        m_taskIndexMap.clear();
        m_taskIndexMap.reserve(m_projects.size());
        for (int row = 0; row < static_cast<int>(m_projects.size()); row++)
                m_taskIndexMap[m_projects[row].id] = row;
}

} // namespace NfDesktop
