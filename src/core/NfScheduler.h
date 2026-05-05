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

#include <mutex>
#include <memory>
#include <function>
#include <set>
#include <unordered_map>

namespace NfCore {

class NfTask;

/**
 * The NfScheduler acts as the central "brain" for task management, separate from the
 * execution threads. It manages task ownership, prioritization, and cancellation.
 *
 * std::unordered_map (Ownership): Provides O(1) average access to tasks by ID.
 * std::set (Priority Index): Acts as a balanced binary search tree for O(log n)
 * sorted access and arbitrary element removal.
 *
 * Time Complexity Guarantees
 *
 * | Operation             | Complexity  | Technical Reason                                     |
 * |-----------------------|-------------|------------------------------------------------------|
 * | submit()              | O(log n)    | Map insertion (O(1)) + Set insertion (O(log n)).     |
 * | nextTask()            | O(log n)    | Get begin() (O(1)) + Set removal (O(log n)).         |
 * | cancelTask()          | O(log n)    | Map lookup (O(1)) + Set arbitrary removal (O(log n)).|
 * | updateTaskPriority()  | O(log n)    | Set erase + Set re-insert (2 * O(log n)).            |
 * | finalizeTask()        | O(1)        | Direct removal from the ownership map.               |
 * | hasTask()             | O(1)        | Hash map lookup.                                     |
 *
 */
class NfScheduler {
 public:
        using TasksAvailableCallback = std::function<void>();

        explicit NfScheduler();
        ~NfScheduler();

        NfScheduler(const NfScheduler&) = delete;
        NfScheduler& operator=(const NfScheduler&) = delete;
        void setTasksAvailableCallback(TasksAvailableCallback callback);
        void submit(std::unique_ptr<NfTask> task);
        NfTask* nextTask();
        void finalizeTask(NfTask::TaskId id);
        void updateTaskPriority(NfTask::TaskId id, NfTask::Priority priority);
        void updateTaskPriority(NfTask::TaskId id, int priority);
        void cancelTask(NfTask::TaskId id);
        void cancelAll();

        size_t pendingTaskCount() const;
        bool hasTask(NfTask::TaskId id) const;

 private:
        TasksAvailableCallback m_tasksAvailableCb;

        // Ownership & Quick Lookup. Maps TaskId to task.
        std::unordered_map<NfTask::TaskId, std::unique_ptr<NfTask>> m_tasks;

        // The Priority Index (The "Active" Queue)
        // We store TaskId and Priority pairs.
        // This allows us to sort by priority and then by TaskId (to stay deterministic).
        struct TaskQueueEntry {
                NfTask::Priority priority;
                int rawPriority; // If using the int overload
                NfTask::TaskId id;

                // Custom comparator for the set/queue
                bool operator<(const QueueEntry& other) const {
                        if (priority != other.priority)
                                return priority > other.priority;
                        if (rawPriority != other.rawPriority)
                                return rawPriority > other.rawPriority;
                        return id < other.id;
                }
        };

        // Using a std::set as a priority queue because it allows O(log n) removal
        // of any element, which is required for cancelTask and updateTaskPriority.
        std::set<TaskQueueEntry> m_priorityQueue;

        mutable std::mutex m_mutex;
        bool m_shutingDown = false;
};

} // namespace NfCore

#endif // NF_SCHEDULER_H
