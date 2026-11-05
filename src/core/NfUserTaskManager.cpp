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
 */

#include "NfUserTaskManager.h"

#include <utility>
#include <vector>

namespace NfCore {

NfUserTaskManager::SubscriptionId
NfUserTaskManager::subscribe(Observer observer)
{
        std::lock_guard lock(m_observersMutex);
        const auto id = m_nextSubscription++;
        m_observers.emplace(id, std::make_shared<ObserverEntry>());
        m_observers.at(id)->observer = std::move(observer);
        return id;
}

void NfUserTaskManager::unsubscribe(SubscriptionId subscription)
{
        std::shared_ptr<ObserverEntry> entry;
        {
                std::lock_guard lock(m_observersMutex);
                const auto it = m_observers.find(subscription);
                if (it == m_observers.end())
                        return;
                entry = std::move(it->second);
                m_observers.erase(it);
        }

        std::lock_guard lock(entry->mutex);
        entry->active = false;
        entry->observer = {};
}

std::vector<std::shared_ptr<NfUserTaskManager::ObserverEntry>>
NfUserTaskManager::observers() const
{
        std::lock_guard lock(m_observersMutex);
        std::vector<std::shared_ptr<ObserverEntry>> result;
        result.reserve(m_observers.size());
        for (const auto& [id, entry] : m_observers) {
                (void)id;
                result.push_back(entry);
        }
        return result;
}

void NfUserTaskManager::taskAdded(TaskId id, NfUserTaskData data)
{
        for (const auto& entry : observers()) {
                std::lock_guard lock(entry->mutex);
                if (entry->active && entry->observer.taskAdded)
                        entry->observer.taskAdded(id, data);
        }
}

void NfUserTaskManager::taskUpdated(TaskId id, NfUserTaskData data)
{
        for (const auto& entry : observers()) {
                std::lock_guard lock(entry->mutex);
                if (entry->active && entry->observer.taskUpdated)
                        entry->observer.taskUpdated(id, data);
        }
}

void NfUserTaskManager::taskRemoved(TaskId id)
{
        for (const auto& entry : observers()) {
                std::lock_guard lock(entry->mutex);
                if (entry->active && entry->observer.taskRemoved)
                        entry->observer.taskRemoved(id);
        }
}

} // namespace NfCore
