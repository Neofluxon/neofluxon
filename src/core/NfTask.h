/**
 * File name: NfTask.h
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

#ifndef NF_TASK_H
#define NF_TASK_H

#include <memory>
#include <atomic>
#include <functional>

namespace NfCore {

class NfTask {
public:
        enum class Priority : int {
                Immediate = 0,
                High      = 20,
                Normal    = 50,
                Low       = 80,
                Idle      = 100
        };

        enum class TaskStatus {
                Success,
                Failed,
                Cancelled
        };

        using TaskId = uint64_t;
        using TaskResultHandler = std::function<void(NfTask*, TaskStatus)>;
        virtual ~NfTask() = default;
        uint64_t taskId() const;
        virtual TaskStatus execute() = 0;
        void setResult(TaskResultHandler handler);
        void notifyCompletion(TaskStatus status = TaskStatus::Success);
        [[nodiscard]] bool hasResultHandler() const;
        void setPriority(Priority p);
        void setPriority(int p);
        int priority() const;
        void setSequence(uint64_t s) { m_sequence = s; }
        uint64_t sequence() const { return m_sequence; }
        virtual void cancel() {}

protected:
        NfTask();

private:
        TaskResultHandler m_onComplete;
        int m_priority = static_cast<int>(Priority::Immediate);
        uint64_t m_sequence = 0;
        static std::atomic<uint64_t> s_taskIdGenerator;
        TaskId m_taskId;
};

} // namespace NfCore

#endif // NF_TASK_H
