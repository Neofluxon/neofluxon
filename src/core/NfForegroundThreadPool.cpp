/**
 * File name: NfForegroundThreadPool.cpp
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

#include "NfForegroundThreadPool.h"
#include "NfLogger.h"
#include "NfTask.h"

namespace NfCore {

NfForegroundThreadPool::NfForegroundThreadPool(size_t threadCount)
{
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

NfForegroundThreadPool::~NfForegroundThreadPool()
{
        for (auto& t : m_poolThreads)
                t.request_stop();

        m_conditionVariable.notify_all();
}

void NfForegroundThreadPool::submit(std::unique_ptr<NfTask> task)
{
        {
                std::scoped_lock<std::mutex> lock(m_queueMutex);
                m_taskQueue.push(std::move(task));
        }

        m_conditionVariable.notify_all();
}

void NfForegroundThreadPool::threadLoop(std::stop_token stoken)
{
        while (true) {
                std::unique_ptr<NfTask> task;

                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_conditionVariable.wait(lock, [this, &stoken] {
                            return !m_taskQueue.empty() || stoken.stop_requested();
                    });

                    if (stoken.stop_requested())
                            break;

                    task = std::move(const_cast<std::unique_ptr<NfTask>&>(m_taskQueue.top()));
                    m_taskQueue.pop();
                }

                if (task) {
                        auto status = task->execute();
                        NF_LOG_DEBUG("status: " << (int)status);
                        task->notifyCompletion(status);
                }
        }
}

bool NfForegroundThreadPool::TaskCompare::operator()(const std::unique_ptr<NfTask>& a,
                                                     const std::unique_ptr<NfTask>& b) const
{
        if (a->priority() != b->priority())
                return a->priority() > b->priority();
        return a->sequence() < b->sequence();
}

} // namespace NfCore
