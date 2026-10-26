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
#include "NfLogger.h"

#include <algorithm>
#include <sstream>

namespace NfCore {

NfScheduler::NfScheduler()
{
}

NfScheduler::~NfScheduler()
{
        // Drop queued work and request cancellation for tasks already executing.
        cancelAll();

        // Keep running-task ownership valid until every worker has finalized.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_runningTasksConditionVariable.wait(lock, [this] {
                return m_runningTasks.empty();
        });
}

void NfScheduler::submit(std::unique_ptr<NfTask> task)
{
        if (!task)
                return;

        {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto id = task->taskId();
                const auto sequence = m_nextSequence++;
                TaskQueueEntry entry{task->priority(), sequence, id};

                // Keep task ownership, its sequence, and the ordering index in sync.
                m_pendingTasks[id] = std::move(task);
                m_pendingTaskSequences[id] = sequence;
                m_priorityQueue.insert(entry);
        }

        // Wake a worker only after releasing the scheduler lock.
        m_taskAvailableConditionVariable.notify_one();
}

NfTask* NfScheduler::waitNextTask(std::stop_token stoken)
{
        std::unique_lock<std::mutex> lock(m_mutex);
        m_taskAvailableConditionVariable.wait(lock, stoken, [this] {
                return !m_priorityQueue.empty();
        });

        // The wait may also return because the worker was asked to stop.
        if (stoken.stop_requested())
                return nullptr;

        // The first set entry has the best priority and newest sequence.
        auto it = m_priorityQueue.begin();
        NfTask::TaskId id = it->id;
        m_priorityQueue.erase(it);
        m_pendingTaskSequences.erase(id);

        // Transfer ownership from the pending set to the running set.
        m_runningTasks.insert({id, std::move(m_pendingTasks[id])});
        m_pendingTasks.erase(id);

        NF_LOG_DEBUG("TAKE ID: " << id);

        return m_runningTasks[id].get();
}

bool NfScheduler::updateTaskPriority(NfTask::TaskId id, NfTask::Priority priority)
{
        return updateTaskPriority(id, static_cast<int>(priority));
}

bool NfScheduler::updateTaskPriority(NfTask::TaskId id, int priority)
{
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_pendingTasks.find(id);
        if (it == m_pendingTasks.end())
                return false;

        // Find the current queue sequence so the existing entry can be removed.
        const auto sequenceIt = m_pendingTaskSequences.find(id);
        if (sequenceIt == m_pendingTaskSequences.end())
                return false;

        m_priorityQueue.erase({it->second->priority(), sequenceIt->second, id});
        it->second->setPriority(priority);
        // A fresh sequence moves the task ahead of older tasks at this priority.
        const auto sequence = m_nextSequence++;
        sequenceIt->second = sequence;

        // Reinsert with the updated ordering metadata; the task ID is unchanged.
        m_priorityQueue.insert({priority, sequence, id});

        return true;
}

void NfScheduler::cancelTask(NfTask::TaskId id)
{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_pendingTasks.find(id);
        if (it == m_pendingTasks.end())
                return;

        // Running tasks are owned by workers and are not removed here.
        const auto sequenceIt = m_pendingTaskSequences.find(id);
        if (sequenceIt != m_pendingTaskSequences.end()) {
                m_priorityQueue.erase({it->second->priority(), sequenceIt->second, id});
                m_pendingTaskSequences.erase(sequenceIt);
        }
        m_pendingTasks.erase(it);
}

void NfScheduler::cancelTasks(const std::vector<NfTask::TaskId>& ids)
{
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto id : ids) {
                auto pendingIt = m_pendingTasks.find(id);
                if (pendingIt != m_pendingTasks.end()) {
                        const auto sequenceIt = m_pendingTaskSequences.find(id);
                        if (sequenceIt != m_pendingTaskSequences.end()) {
                                m_priorityQueue.erase(
                                        {pendingIt->second->priority(), sequenceIt->second, id});
                                m_pendingTaskSequences.erase(sequenceIt);
                        }
                        m_pendingTasks.erase(pendingIt);
                        continue;
                }

                const auto runningIt = m_runningTasks.find(id);
                if (runningIt != m_runningTasks.end())
                        runningIt->second->cancel();
        }
}

void NfScheduler::cancelAll()
{
        std::lock_guard<std::mutex> lock(m_mutex);
        // Pending work is discarded; running tasks are asked to stop cooperatively.
        m_pendingTasks.clear();
        m_pendingTaskSequences.clear();
        m_priorityQueue.clear();

        for (auto &task: m_runningTasks)
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
        bool noRunningTasks;
        {
                std::lock_guard<std::mutex> lock(m_mutex);
                // Removing the task releases scheduler ownership after execution.
                m_runningTasks.erase(id);
                noRunningTasks = m_runningTasks.empty();
        }

        // Notify outside the lock so the destructor can reacquire it.
        if (noRunningTasks)
                m_runningTasksConditionVariable.notify_all();
}

} // namespace NfCore
