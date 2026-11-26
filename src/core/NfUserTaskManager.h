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
#include "NfObserverRegistry.h"

#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace NfCore {

class NfUserTaskManager : public NfObserverRegistry<NfUserTaskManager::Observer> {
public:
        using TaskId = NfTask::TaskId;

        struct Observer {
                std::function<void(TaskId, NfUserTaskData)> taskAdded;
                std::function<void(TaskId, NfUserTaskData)> taskUpdated;
                std::function<void(TaskId)> taskRemoved;
        };

        NfUserTaskManager() = default;
        ~NfUserTaskManager();
        NfUserTaskManager(const NfUserTaskManager&) = delete;
        NfUserTaskManager& operator=(const NfUserTaskManager&) = delete;
        void taskAdded(TaskId id, NfUserTaskData data);
        void taskUpdated(TaskId id, NfUserTaskData data);
        void taskRemoved(TaskId id);
};

} // namespace NfCore

#endif // NF_USER_TASK_MANAGER_H
