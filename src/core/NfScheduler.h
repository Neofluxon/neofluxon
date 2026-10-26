/**
 * File name: NfScheduler.h
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

#ifndef NF_SCHEDULER_H
#define NF_SCHEDULER_H

#include "NfTask.h"

#include <mutex>
#include <memory>
#include <set>
#include <unordered_map>
#include <condition_variable>
#include <stop_token>
#include <vector>

namespace NfCore {

/**
 * Owns submitted tasks and coordinates their execution by worker threads.
 *
 * Pending tasks are indexed by task ID for lookup and by a priority queue for
 * selection. Lower numeric priorities run first. Within the same priority,
 * tasks with the newest sequence run first. A pending task's sequence is
 * refreshed when its priority is updated, without changing its task ID.
 *
 * Once selected, a task moves to the running-task map and remains owned by the
 * scheduler until a worker calls finalizeTask(). The scheduler mutex protects
 * all task maps, queue entries, and sequence values.
 *
 * The pending and running unordered maps provide average O(1) lookup by task
 * ID. The ordered set provides O(1) access to the next task and O(log n)
 * insertion/removal, including removal of an arbitrary pending task.
 *
 * Time Complexity Guarantees
 *
 * | Operation           | Complexity
 * |---------------------|---------------------------------------------------------------|
 * | submit()            | O(log n) Set insertion; average O(1) map insertions.          |
 * | waitNextTask()      | O(log n) Set removal; average O(1) map operations.            |
 * | cancelTask()        | O(log n) Set removal; average O(1) map lookup/removal.        |
 * | updateTaskPriority()| O(log n) Set erase + Set re-insert (2 * O(log n)).            |
 * | finalizeTask()      | Average O(1) removal from the running-task map.               |
 * | isTaskPending()     | Average O(1) pending-task map lookup.                         |
 *
 */
class NfScheduler {
 public:
        explicit NfScheduler();
        ~NfScheduler();

        NfScheduler(const NfScheduler&) = delete;
        NfScheduler& operator=(const NfScheduler&) = delete;

        // Takes ownership of a task and assigns its initial queue sequence.
        void submit(std::unique_ptr<NfTask> task);

        // Blocks until a task is available or stop is requested. The returned
        // task remains scheduler-owned until finalizeTask() is called.
        NfTask* waitNextTask(std::stop_token stoken);

        // Updates a pending task's priority and sequence; returns false if it
        // is no longer pending.
        bool updateTaskPriority(NfTask::TaskId id, NfTask::Priority priority);
        bool updateTaskPriority(NfTask::TaskId id, int priority);

        // Removes a pending task, or cancels all running tasks and removes all
        // pending tasks.
        void cancelTask(NfTask::TaskId id);
        void cancelTasks(const std::vector<NfTask::TaskId>& ids);
        void cancelAll();

        size_t pendingTaskCount() const;
        bool hasPendingTasks() const;
        bool isTaskPending(NfTask::TaskId id) const;

        // Releases scheduler ownership of a completed running task.
        void finalizeTask(NfTask::TaskId id);

 private:
        // Task ID is stable identity; sequence is separate queue-order metadata.
        std::unordered_map<NfTask::TaskId, std::unique_ptr<NfTask>> m_pendingTasks;
        std::unordered_map<NfTask::TaskId, std::unique_ptr<NfTask>> m_runningTasks;

        // Smaller priority values come first; newer sequences come first for
        // equal priorities. Task ID is a deterministic final tie-breaker.
        struct TaskQueueEntry {
                int priority;
                uint64_t sequence;
                NfTask::TaskId id;

                bool operator<(const TaskQueueEntry& other) const {
                        if (priority != other.priority)
                                return priority < other.priority;
                        if (sequence != other.sequence)
                                return sequence > other.sequence;
                        return id < other.id;
                }
        };

        // The sequence map lets us erase a pending task's exact queue entry.
        std::set<TaskQueueEntry> m_priorityQueue;
        std::unordered_map<NfTask::TaskId, uint64_t> m_pendingTaskSequences;

        // Monotonically assigns queue-order values while holding m_mutex.
        uint64_t m_nextSequence{0};

        mutable std::mutex m_mutex;
        std::condition_variable_any m_taskAvailableConditionVariable;
        std::condition_variable m_runningTasksConditionVariable;
};

} // namespace NfCore

#endif // NF_SCHEDULER_H
