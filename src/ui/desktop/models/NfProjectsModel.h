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

#ifndef NF_PROJECTS_MODEL_H
#define NF_PROJECTS_MODEL_H

#include "core/NfImportFolderTaskData.h"
#include "core/NfTask.h"
#include "core/NfUserTaskData.h"

#include <QAbstractListModel>
#include <QMetaType>

#include <cstddef>
#include <unordered_map>
#include <vector>

Q_DECLARE_METATYPE(NfCore::NfProjectData)

namespace NfUi {
class NfProjectsAdapter;
}

namespace NfDesktop {

class NfProjectsModel : public QAbstractListModel
{
        Q_OBJECT

public:
        enum DataRole {
                ProjectDataRole = Qt::UserRole + 1
        };

        explicit NfProjectsModel(NfUi::NfProjectsAdapter* adapter,
                                 QObject* parent = nullptr);
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index,
                      int role = Qt::DisplayRole) const override;

private:
        NfUi::NfUserTasksAdapter* m_adapter;
        std::vector<NfCore::NfProjectData> m_projects;
};

} // namespace NfDesktop

#endif // NF_PROJECTS_MODEL_H
