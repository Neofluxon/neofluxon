/**
 * File name: NfScheduler.cpp
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

#include "NfScheduler.h"

#include <algorithm>

namespace NfCore {

NfScheduler::NfScheduler()
{
}

NfScheduler::~NfScheduler()
{
        cancelAll();

        // Wait for worker threads to call finalizeTask()
        // for already-running tasks.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_conditionVariable.wait(lock, [this] {
                return m_runningTasks.empty();
        });
}

void NfScheduler::setTasksAvailableCallback(TasksAvailableCallback callback)
{
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasksAvailableCb = std::move(callback);
}

void NfScheduler::submit(std::unique_ptr<NfTask> task)
{
        if (!task)
                return;

        std::lock_guard<std::mutex> lock(m_mutex);

        auto id = task->taskId();
        TaskQueueEntry entry{task->priority(), id};

        // Store ownership
        m_pendingTasks[id] = std::move(task);
        // Index for priority ordering
        m_priorityQueue.insert(entry);

        if (m_tasksAvailableCb)
                m_tasksAvailableCb();
}

NfTask* NfScheduler::nextTask()
{
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_priorityQueue.empty())
                return nullptr;

        // Extract the highest priority entry
        auto it = m_priorityQueue.begin();
        NfTask::TaskId id = it->id;
        m_priorityQueue.erase(it);

        m_runningTasks.insert({id, std::move(m_pendingTasks[id])});
        m_pendingTasks.erase(id);

        return m_runningTasks[id].get();
}

bool NfScheduler::updateTaskPriority(NfTask::TaskId id, NfTask::Priority priority)
{
        return updateTaskPriority(id, static_cast<int>(priority));
}

bool NfScheduler::updateTaskPriority(NfTask::TaskId id, int priority)
{
        bool updated = false;

        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_pendingTasks.find(id);
        if (it != m_pendingTasks.end()) {
                m_priorityQueue.erase({it->second->priority(), id});
                it->second->setPriority(priority);
                m_priorityQueue.insert({it->second->priority(), id});
                updated = true;
        }

        it = m_runningTasks.find(id);
        if (it != m_runningTasks.end()) {
                it->second->setPriority(priority);
                updated = true;
        }

        return true;
}

void NfScheduler::cancelTask(NfTask::TaskId id)
{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_pendingTasks.find(id);
        if (it == m_pendingTasks.end())
                return;

        m_priorityQueue.erase({it->second->priority(), id});
        m_pendingTasks.erase(it);
}

void NfScheduler::cancelAll()
{
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pendingTasks.clear();
        m_priorityQueue.clear();

        for (auto &task: m_pendingTasks)
                task.second->cancel();
}

size_t NfScheduler::pendingTaskCount() const
{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_priorityQueue.size();
}

bool NfScheduler::hasPendingTasks() const
{
        return pendingTaskCount() > 0;
}

bool NfScheduler::isTaskPending(NfTask::TaskId id) const
{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pendingTasks.find(id) != m_pendingTasks.end();
}

void NfScheduler::finalizeTask(NfTask::TaskId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_runningTasks.erase(id);

    if (m_runningTasks.empty())
            m_conditionVariable.notify_all();
}

} // namespace NfCore
