/**
 * File name: NfTask.cpp
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

#include "NfTask.h"

namespace NfCore {

std::atomic<uint64_t> NfTask::s_taskIdGenerator{0};

NfTask::NfTask()
        : m_taskId{s_taskIdGenerator.fetch_add(1, std::memory_order_relaxed)}
{
}

uint64_t NfTask::taskId() const
{
        return m_taskId;
}

void NfTask::setResult(NfTask::TaskResultHandler handler)
{
        m_onComplete = std::move(handler);
}

void NfTask::notifyCompletion(NfTask::TaskStatus status)
{
        if (!m_onComplete)
                return;

        m_onComplete(this, status);
}

bool NfTask::hasResultHandler() const
{
        return static_cast<bool>(m_onComplete);
}

void NfTask::setPriority(NfTask::Priority p)
{
        m_priority = static_cast<int>(p);
}

void NfTask::setPriority(int p)
{
        m_priority = p;
}

int NfTask::priority() const
{
        return m_priority;
}

void NfTask::cancel()
{
        m_cancelled.store(true, std::memory_order_relaxed);
}

bool NfTask::isCancelled() const
{
        return m_cancelled.store(true, std::memory_order_relaxed);
}

} // namespace NfCore
