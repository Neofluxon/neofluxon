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
 */

#ifndef NF_USER_TASKS_MODEL_H
#define NF_USER_TASKS_MODEL_H

#include "core/NfImportFolderTaskData.h"
#include "core/NfTask.h"
#include "core/NfUserTaskData.h"

#include <QAbstractListModel>

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace NfUi {
class NfUserTasksAdapter;
}

namespace NfDesktop {

class NfUserTasksModel : public QAbstractListModel
{
        Q_OBJECT

public:
        enum DataRole {
                TaskIdRole = Qt::UserRole + 1,
                TaskTypeRole,
                SourcePathRole,
                StatusRole,
                DiscoveredPhotosRole,
                ProcessedPhotosRole,
                ImportedPhotosRole,
                SkippedPhotosRole,
                FailedPhotosRole
        };

        explicit NfUserTasksModel(NfUi::NfUserTasksAdapter* adapter,
                                  QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index,
                      int role = Qt::DisplayRole) const override;
        int count(NfCore::NfImportFolderTaskData::Status status) const;

private:
        struct TaskItem {
                NfCore::NfTask::TaskId id;
                NfCore::NfUserTaskData data;
        };

        int rowForTaskId(NfCore::NfTask::TaskId id) const;
        void onTaskAdded(qulonglong id, NfCore::NfUserTaskData data);
        void onTaskUpdated(qulonglong id, NfCore::NfUserTaskData data);
        void onTaskRemoved(qulonglong id);

        NfUi::NfUserTasksAdapter* m_adapter;
        std::vector<TaskItem> m_tasks;
        std::unordered_map<NfCore::NfTask::TaskId, NfCore::NfUserTaskType> m_taskTypes;
};

} // namespace NfDesktop

#endif // NF_USER_TASKS_MODEL_H
