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
        : m_shuttingDown{false}
{
}

NfScheduler::~NfScheduler()
{
        cancelAll();
}

void NfScheduler::submit(std::unique_ptr<NfTask> task)
{
        if (!task)
                return;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_shuttingDown)
                return;

        NfTask::TaskId id = task->id();
        QueueEntry entry{task->priority(), task->rawPriority(), id};

        // Store ownership
        m_tasks[id] = std::move(task);
        // Index for priority ordering
        m_priorityQueue.insert(entry);

        m_condition.notify_one();
}

NfTask* NfScheduler::nextTask()
{
        std::unique_lock<std::mutex> lock(m_mutex);

        // Wait until queue is not empty or we are shutting down
        m_condition.wait(lock, [this] {
                return !m_priorityQueue.empty() || m_shuttingDown;
        });

        if (m_shuttingDown && m_priorityQueue.empty())
                return nullptr;

        // Extract the highest priority entry
        auto it = m_priorityQueue.begin();
        NfTask::TaskId id = it->id;
        m_priorityQueue.erase(it);

        // Return the raw pointer. Ownership remains in m_tasks.
        return m_tasks[id].get();
}

void NfScheduler::finalizeTask(NfTask::TaskId id)
{
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.erase(id);
}

void NfScheduler::updateTaskPriority(NfTask::TaskId id, NfTask::Priority priority)
{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_tasks.find(id);
        if (it == m_tasks.end())
                return;

        // Remove old entry from the priority index
        m_priorityQueue.erase({it->second->priority(), it->second->rawPriority(), id});

        // Update the task data
        it->second->setPriority(priority);

        // Re-insert with new priority
        m_priorityQueue.insert({priority, it->second->rawPriority(), id});
}

void NfScheduler::updateTaskPriority(NfTask::TaskId id, int priority)
{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_tasks.find(id);
        if (it == m_tasks.end())
                return;

        m_priorityQueue.erase({it->second->priority(), it->second->rawPriority(), id});
        it->second->setRawPriority(priority);
        m_priorityQueue.insert({it->second->priority(), priority, id});
}

void NfScheduler::cancelTask(NfTask::TaskId id)
{
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_tasks.find(id);
        if (it == m_tasks.end())
                return;

        m_priorityQueue.erase({it->second->priority(), it->second->rawPriority(), id});
        m_tasks.erase(it);
}

void NfScheduler::cancelAll()
{
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shuttingDown = true;
        m_priorityQueue.clear();
        m_tasks.clear();
        m_condition.notify_all();
}

size_t NfScheduler::pendingTaskCount() const
{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_priorityQueue.size();
}

bool NfScheduler::hasTask(NfTask::TaskId id) const
{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.find(id) != m_tasks.end();
}

} // namespace NfCore
