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
 */

#ifndef NF_USER_TASK_MANAGER_H
#define NF_USER_TASK_MANAGER_H

#include "NfTask.h"
#include "NfUserTaskData.h"

#include <functional>
#include <memory>
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
        void unsubscribe(SubscriptionId subscription);

        void taskAdded(TaskId id, NfUserTaskData data);
        void taskUpdated(TaskId id, NfUserTaskData data);
        void taskRemoved(TaskId id);

private:
        struct ObserverEntry {
                std::mutex mutex;
                bool active{true};
                Observer observer;
        };

        std::vector<std::shared_ptr<ObserverEntry>> observers() const;

        mutable std::mutex m_observersMutex;
        std::unordered_map<SubscriptionId, std::shared_ptr<ObserverEntry>> m_observers;
        SubscriptionId m_nextSubscription{1};
};

} // namespace NfCore

#endif // NF_USER_TASK_MANAGER_H
