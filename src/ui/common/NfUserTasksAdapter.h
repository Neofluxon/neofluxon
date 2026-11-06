/**
 * File name: NfUserTasksAdapter.h
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

#ifndef NF_USER_TASKS_ADAPTER_H
#define NF_USER_TASKS_ADAPTER_H

#include "core/NfTask.h"
#include "core/NfUserTaskData.h"
#include "core/NfUserTaskManager.h"

#include <QObject>

namespace NfUi {

class NfUserTasksAdapter : public QObject
{
        Q_OBJECT

public:
        explicit NfUserTasksAdapter(NfCore::NfUserTaskManager* taskManager,
                                    QObject* parent = nullptr);
        ~NfUserTasksAdapter() override;

signals:
        void taskAdded(NfCore::NfTask::TaskId id, NfCore::NfUserTaskData data);
        void taskUpdated(NfCore::NfTask::TaskId, NfCore::NfUserTaskData data);
        void taskRemoved(NfCore::NfTask::TaskId id);

private:
        NfCore::NfUserTaskManager* m_taskManager;
        NfCore::NfUserTaskManager::SubscriptionId m_subscription{};
};

} // namespace NfUi

#endif // NF_USER_TASKS_ADAPTER_H
