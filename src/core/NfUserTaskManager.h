/**
 * File name: NfUserTaskManager.h
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

#ifndef NF_USER_TASK_MANAGER_H
#define NF_USER_TASK_MANAGER_H

#include "NfTask.h"
#include "NfUserTaskData.h"

#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace NfCore {

class NfUserTaskManager {
public:
        using TaskId = NfTask::TaskId;
        using SubscriptionId = uint64_t;

        struct Observer {
                std::function<void(TaskId, NfUserTaskData)> taskAdded;
                std::function<void(TaskId, NfUserTaskData)> taskUpdated;
                std::function<void(TaskId)> taskRemoved;
        };

        NfUserTaskManager() = default;
        NfUserTaskManager(const NfUserTaskManager&) = delete;
        NfUserTaskManager& operator=(const NfUserTaskManager&) = delete;
        SubscriptionId subscribe(Observer observer);
        void unsubscribe(SubscriptionId id);
        void taskAdded(TaskId id, NfUserTaskData data);
        void taskUpdated(TaskId id, NfUserTaskData data);
        void taskRemoved(TaskId id);

private:
        mutable std::mutex m_observersMutex;
        std::unordered_map<SubscriptionId, Observer> m_observers;
        SubscriptionId m_subscriptionGenerator{};
};

} // namespace NfCore

#endif // NF_USER_TASK_MANAGER_H
