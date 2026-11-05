/**
 * File name: NfUserTasksAdapter.cpp
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

#include "NfUserTasksAdapter.h"

#include "core/NfUserTaskManager.h"

#include <QMetaObject>
#include <utility>

namespace NfUi {

NfUserTasksAdapter::NfUserTasksAdapter(
        NfCore::NfUserTaskManager* taskManager,
        QObject* parent)
        : QObject(parent)
        , m_taskManager{taskManager}
        , m_subscription{0}
{
        Q_ASSERT(m_taskManager);
        m_subscription = m_taskManager->subscribe({
                .taskAdded = [this](NfCore::NfUserTaskManager::TaskId id,
                                    NfCore::NfUserTaskData data) {
                        QMetaObject::invokeMethod(
                                this,
                                [this, id, data = std::move(data)]() {
                                        emit taskAdded(id, data);
                                },
                                Qt::QueuedConnection);
                },
                .taskUpdated = [this](NfCore::NfUserTaskManager::TaskId id,
                                      NfCore::NfUserTaskData data) {
                        QMetaObject::invokeMethod(
                                this,
                                [this, id, data = std::move(data)]() {
                                        emit taskUpdated(id, data);
                                },
                                Qt::QueuedConnection);
                },
                .taskRemoved = [this](NfCore::NfUserTaskManager::TaskId id) {
                        QMetaObject::invokeMethod(
                                this,
                                [this, id]() { emit taskRemoved(id); },
                                Qt::QueuedConnection);
                }
        });
}

NfUserTasksAdapter::~NfUserTasksAdapter()
{
        m_taskManager->unsubscribe(m_subscription);
}

} // namespace NfUi
