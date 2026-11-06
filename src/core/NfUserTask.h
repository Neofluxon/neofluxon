/**
 * File name: NfUserTask.h
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

#ifndef NF_USER_TASK_H
#define NF_USER_TASK_H

#include "NfTask.h"
#include "NfUserTaskManager.h"

#include <utility>

namespace NfCore {

class NfUserTask : public NfTask {
public:
        using TaskId = NfTask::TaskId;
        explicit NfUserTask(NfUserTaskManager* taskManager)
                : m_taskManager{taskManager}
        {
        }
        ~NfUserTask() override = default;

protected:
        void notifyTaskAdded(NfUserTaskData data)
        {
                m_taskManager->taskAdded(taskId(), std::move(data));
        }

        void notifyTaskUpdated(NfUserTaskData data)
        {
                m_taskManager->taskUpdated(taskId(), std::move(data));
        }

        void notifyTaskRemoved()
        {
                m_taskManager->taskRemoved(taskId());
        }

private:
        NfUserTaskManager* m_taskManager;
};

} // namespace NfCore

#endif // NF_USER_TASK_H
