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
#include "NfContext.h"
#include "NfDesktopContext.h"
#include "NfProjectsAdapter.h"
#include "core/NfLogger.h"

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfProjectsModel::NfProjectsModel(const NfDesktopContext &ctx,
                                 QObject* parent)
        : QAbstractListModel(parent)
        , m_context{ctx}
        , m_adapter{m_context.uiContext->projectsAdapter}
{
        QObject::connect(m_adapter,
                         &NfProjectsAdapter::projectsListUpdated,
                         this,
                         &NfProjectsModel::onProjectsListUpdated);
        QObject::connect(m_adapter,
                         &NfProjectsAdapter::projectAdded,
                         this,
                         &NfProjectsModel::onProjectAdded);
        QObject::connect(m_adapter,
                         &NfProjectsAdapter::projectUpdated,
                         this,
                         &NfProjectsModel::onProjectUpdated);
        QObject::connect(m_adapter,
                         &NfProjectsAdapter::projectRemoved,
                         this,
                         &NfProjectsModel::onProjectRemoved);

        m_adapter->requestProjects();
}

int NfProjectsModel::rowCount(const QModelIndex& parent) const
{
        return parent.isValid() ? 0 : static_cast<int>(m_projects.size());
}

QVariant NfProjectsModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid()
            || static_cast<std::size_t>(index.row()) >= m_projects.size()) {
                return {};
        }

        switch (role) {
        case Qt::DisplayRole:
                return QString::fromStdString(m_projects[static_cast<std::size_t>(index.row())].name);
        case ProjectDataRole:
                return QVariant::fromValue(m_projects[static_cast<std::size_t>(index.row())]);
        }

        return {};
}

void NfProjectsModel::onProjectsListUpdated(std::vector<NfProjectData> projects)
{
        beginResetModel();
        m_projects = std::move(projects);
        endResetModel();
        rebuildIndexMap();
}

void NfProjectsModel::onProjectAdded(NfProjectData project)
{
        const auto row = rowByProjectId(project.id);
        if (row >= 0) {
                updateProjectItem(row, project);
                return;
        }

        // Insert new item
        const auto newRow = static_cast<int>(m_projects.size());
        beginInsertRows({}, newRow, newRow);
        m_projects.push_back(std::move(project));
        endInsertRows();

        rebuildIndexMap();
}

void NfProjectsModel::onProjectUpdated(NfProjectData project)
{
        const auto row = rowByProjectId(project.id);
        if (row < 0)
                return;

        updateProjectItem(row, std::move(project));
}

void NfProjectsModel::onProjectRemoved(int64_t id)
{
        const auto row = rowByProjectId(id);
        if (row < 0)
                return;

        beginRemoveRows({}, row, row);
        m_projects.erase(m_projects.begin() + row);
        endRemoveRows();

        rebuildIndexMap();
}

void NfProjectsModel::updateProjectItem(int row, NfProjectData project)
{
        const auto itemIndex = index(row);
        if (!itemIndex.isValid())
                return;

        m_projects[row] = std::move(project);
        emit dataChanged(itemIndex, itemIndex, {Qt::DisplayRole});
}

int NfProjectsModel::rowByProjectId(int64_t id) const
{
        auto res = m_projectIndexMap.find(id);
        if (res != m_projectIndexMap.end())
                return res->second;

        return -1;
}

void NfProjectsModel::rebuildIndexMap()
{
        m_projectIndexMap.clear();
        m_projectIndexMap.reserve(m_projects.size());
        for (int row = 0; row < static_cast<int>(m_projects.size()); row++)
                m_projectIndexMap[m_projects[row].id] = row;
}

} // namespace NfDesktop
