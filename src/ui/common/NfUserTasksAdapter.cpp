/**
 * File name: NfUserTasksAdapter.h
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

#include "NfUserTasksAdapter.h"
#include "core/NfUserTaskManager.h"

#include <utility>

using namespace NfCore;

namespace NfUi {

NfUserTasksAdapter::NfUserTasksAdapter(NfUserTaskManager* taskManager,
                                       QObject* parent)
        : QObject(parent)
        , m_taskManager{taskManager}
{
        m_subscription = m_taskManager->subscribe({
                        .taskAdded = [this](NfTask::TaskId id,
                                            NfUserTaskData data) {
                                emit taskAdded(id, std::move(data));
                        },
                        .taskUpdated = [this](NfTask::TaskId id,
                                              NfUserTaskData data) {
                                emit taskUpdated(id, std::move(data));
                        },
                        .taskRemoved = [this](NfTask::TaskId id) {
                                emit taskRemoved(id);
                        }
        });
}

NfUserTasksAdapter::~NfUserTasksAdapter()
{
        m_taskManager->unsubscribe(m_subscriptionId);
}

} // namespace NfUi
