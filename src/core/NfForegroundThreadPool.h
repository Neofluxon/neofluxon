/**
 * File name: NfForegroundThreadPool.h
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

#ifndef NF_FOREGROUND_THREAD_POOL_H
#define NF_FOREGROUND_THREAD_POOL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <stop_token>
#include <vector>
#include <atomic>
#include <memory>
#include <cstddef>
#include <vector>

namespace NfCore {

class NfScheduler;

class NfForegroundThreadPool {
public:
        explicit NfForegroundThreadPool(NfScheduler *scheduler,
                                        size_t threadCount = 0);
        ~NfForegroundThreadPool();
        NfForegroundThreadPool(const NfForegroundThreadPool&) = delete;
        NfForegroundThreadPool& operator=(const NfForegroundThreadPool&) = delete;
        NfForegroundThreadPool(NfForegroundThreadPool&&) = delete;
        NfForegroundThreadPool& operator=(NfForegroundThreadPool&&) = delete;

private:
        void threadLoop(std::stop_token stoken);

        NfScheduler *m_scheduler;
        std::vector<std::jthread> m_poolThreads;
        mutable std::mutex m_mutex;
        std::condition_variable_any m_conditionVariable;
};

} // namespace NfCore

#endif // NF_FOREGROUND_THREAD_POOL_H
