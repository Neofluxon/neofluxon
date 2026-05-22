/**
 * File name: NfThreadPool.cpp
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

#include "NfThreadPool.h"
#include "NfScheduler.h"
#include "NfLogger.h"
#include "NfTask.h"

namespace NfCore {

NfThreadPool::NfThreadPool(NfScheduler *scheduler,
                                               size_t threadCount)
        : m_scheduler{scheduler}
{
        m_scheduler->setTasksAvailableCallback([this]() {
                m_conditionVariable.notify_one();
        });

        if (threadCount < 1) {
                threadCount = std::thread::hardware_concurrency();
                if (threadCount < 1)
                        threadCount = 1;
        }

        for (size_t i = 0; i < threadCount; i++) {
                m_poolThreads.emplace_back([this](std::stop_token stoken) {
                        threadLoop(stoken);
                });
        }
}

NfThreadPool::~NfThreadPool()
{
        m_scheduler->setTasksAvailableCallback({});

        for (auto& t : m_poolThreads)
                t.request_stop();

        m_conditionVariable.notify_all();
}

void NfThreadPool::threadLoop(std::stop_token stoken)
{
        while (true) {
                NfTask* task = nullptr;

                {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_conditionVariable.wait(lock, [this, &stoken] {
                                return stoken.stop_requested()
                                        || m_scheduler->hasPendingTasks();
                        });

                        if (stoken.stop_requested())
                                break;

                        task = m_scheduler->nextTask();
                }

                if (task) {
                        auto status = task->execute();

                        NF_LOG_DEBUG("status: " << (int)status);

                        task->notifyCompletion(status);
                        m_scheduler->finalizeTask(task->taskId());
                }
        }
}

} // namespace NfCore
