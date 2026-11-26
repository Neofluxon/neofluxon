/**
 * File name: NfObserverRegistry.h
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

#ifndef NF_OBSERVER_REGISTRY_H
#define NF_OBSERVER_REGISTRY_H

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace NfCore {

template <typename Observer>
class NfObserverRegistry
{
 public:
        using SubscriptionId = uint64_t;
        NfObserverRegistry() = default;
        virtual ~NfObserverRegistry()
        {
                std::lock_guard lock(m_observersMutex);
                m_observers.clear();
        };

        SubscriptionId subscribe(Observer observer)
        {
                std::lock_guard lock(m_observersMutex);
                const auto id = m_subscriptionGenerator++;
                m_observers.emplace(id, std::move(observer));
                return id;
        }

        void unsubscribe(SubscriptionId id)
        {
                std::lock_guard lock(m_observersMutex);
                m_observers.erase(id);
        }

        template<typename Callback>
        void notifyObservers(Callback&& callback) const
        {
                // Hold the lock during callback.
                // An Observer doesn't subscribe/unsibscribe
                // in the callback.
                std::lock_guard lock(m_observersMutex);
                for (const auto& [id, observer] : m_observers)
                        callback(observer);
        }

 private:
        mutable std::mutex m_observersMutex;
        std::unordered_map<SubscriptionId, Observer> m_observers;
        SubscriptionId m_subscriptionGenerator{};
};

} // namspace NfCore

#endif // NF_OBSERVER_REGISTRY_H
