/**
 * File name: NfBagroundTasksModel.h
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

#ifndef NF_BAGROUND_TASKS_MODEL_H
#define NF_BAGROUND_TASKS_MODEL_H

#include "NfBackgroundTask.h"

#include <QAbstractListModel>

#include <cstddef>
#include <vector>

namespace NfUi {
class NfBackgroundTasksAdapter;
}

namespace NfDesktop {

class NfBagroundTasksModel : public QAbstractListModel
{
        Q_OBJECT

public:
        enum DataRole {
                OperationRole = Qt::UserRole + 1,
                SourcePathRole,
                StateRole,
                ProgressRole,
                ProgressTextRole,
                ActionTextRole,
                TaskIdRole
        };

        explicit NfBagroundTasksModel(NfUi::NfBackgroundTasksAdapter* adapter,
                                      QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index,
                      int role = Qt::DisplayRole) const override;
        int count(NfUi::NfBackgroundTask::State state) const;
        void requestAction(const QModelIndex& index);

private:
        void onTaskAdded(const NfUi::NfBackgroundTask& task);
        void onTaskUpdated(const NfUi::NfBackgroundTask& task);
        void onTaskRemoved(const QString& taskId);
        int rowForTaskId(const QString& taskId) const;

        NfUi::NfBackgroundTasksAdapter* m_adapter;
        std::vector<NfUi::NfBackgroundTask> m_tasks;
};

} // namespace NfDesktop

#endif // NF_BAGROUND_TASKS_MODEL_H
