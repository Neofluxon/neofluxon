/**
 * File name: NfUserTaskManager.cpp
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

#include "NfUserTaskManager.h"

#include <utility>
#include <vector>

namespace NfCore {

NfUserTaskManager::SubscriptionId
NfUserTaskManager::subscribe(Observer observer)
{
        std::lock_guard lock(m_observersMutex);
        const auto id = m_subscriptionGenerator++;
        m_observers.emplace(id, std::move(observer));
        return id;
}

void NfUserTaskManager::unsubscribe(SubscriptionId id)
{
        std::lock_guard lock(m_observersMutex);
        m_observers.erase(id);
}

NfUserTaskManager::~NfUserTaskManager()
{
        std::lock_guard lock(m_observersMutex);
        m_observers.clear();
}

void NfUserTaskManager::taskAdded(TaskId id, NfUserTaskData data)
{
        std::lock_guard lock(m_observersMutex);
        for (auto& [subscriptionId, observer] : m_observers) {
                if (observer.taskAdded)
                        observer.taskAdded(id, data);
        }
}

void NfUserTaskManager::taskUpdated(TaskId id, NfUserTaskData data)
{
        std::lock_guard lock(m_observersMutex);
        for (auto& [subscriptionId, observer] : m_observers) {
                if (observer.taskUpdated)
                        observer.taskUpdated(id, data);
        }
}

void NfUserTaskManager::taskRemoved(TaskId id)
{
        std::lock_guard lock(m_observersMutex);
        for (auto& [subscriptionId, observer] : m_observers) {
                if (observer.taskRemoved)
                        observer.taskRemoved(id);
        }
}

} // namespace NfCore
