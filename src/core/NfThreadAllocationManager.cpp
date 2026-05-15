/**
 * File name: NfThreadAllocationManager.h
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

#include "NfThreadAllocationManager.h"

#include <thread>
#include <algorithm>

namespace NfCore {

NfThreadAllocationManager::NfThreadAllocationManager(unsigned int overrideHardwareConcurrency) noexcept
{
        m_totalAvailableThreads = overrideHardwareConcurrency > 0
                ? overrideHardwareConcurrency
                : std::thread::hardware_concurrency();

        if (m_totalAvailableThreads == 0)
                m_totalAvailableThreads = 4;

        calculateAllocation();
}

unsigned int NfThreadAllocationManager::getTotalThreads() const noexcept
{
        return m_totalAvailableThreads;
}

unsigned int NfThreadAllocationManager::getForegroundThreadCount() const noexcept
{
        return m_foregroundThreads;
}

unsigned int NfThreadAllocationManager::getBackgroundThreadCount() const noexcept
{
        return m_backgroundThreads;
}

void NfThreadAllocationManager::calculateAllocation() noexcept
{
        if (m_totalAvailableThreads <= 4) {
                m_foregroundThreads = std::max(1U, m_totalAvailableThreads - 1);
                m_backgroundThreads = 1;
        } else {
                // Two threads for background tasks.
                m_backgroundThreads = 2;

                /*
                 * Account for 4 threads outside the foreground pool:
                 * - 2 Background threads
                 * - 1 Dedicated File Scanner thread
                 * - 1 GUI / OS overhead thread
                 */
                m_foregroundThreads = m_totalAvailableThreads - 4;
        }

        NF_LOG_DEBUG("Thread allocation: "
                     << "total: " << m_totalAvailableThreads << "\n"
                     << "foreground: " << m_foregroundThreads << "\n"
                     << "background: " << m_backgroundThreads);
}

} // namespace NfCore
