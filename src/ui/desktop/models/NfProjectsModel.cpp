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
#include "NfDesktopContext.h"
//#include "NfProjectsAdapter.h"

using namespace NfCore;

namespace NfDesktop {

NfProjectsModel::NfProjectsModel(const NfDesktopContext &ctx,
                                 QObject* parent)
        : QAbstractListModel(parent)
        , m_context{ctx}
{
        /*QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectAdded,
                         this, &NfProjectsModel::onProjectAdded);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectUpdated,
                         this, &NfProjectsModel::onProjectUpdated);
        QObject::connect(m_adapter, &NfUi::NfUserTasksAdapter::projectRemoved,
        this, &NfProjectsModel::onProjectRemoved);*/
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
                return {};
        case ProjectDataRole:
                return QVariant::fromValue(m_projects[static_cast<std::size_t>(index.row())]);
        }

        return {};
}

} // namespace NfDesktop
