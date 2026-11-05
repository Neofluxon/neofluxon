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
 */

#ifndef NF_USER_TASKS_ADAPTER_H
#define NF_USER_TASKS_ADAPTER_H

#include "core/NfUserTaskData.h"
#include "core/NfUserTaskManager.h"

#include <QObject>

namespace NfUi {

class NfUserTasksAdapter : public QObject
{
        Q_OBJECT

public:
        explicit NfUserTasksAdapter(NfCore::NfUserTaskManager* taskManager,
                                    QObject* parent = nullptr);
        ~NfUserTasksAdapter() override;

signals:
        void taskAdded(qulonglong id, NfCore::NfUserTaskData data);
        void taskUpdated(qulonglong id, NfCore::NfUserTaskData data);
        void taskRemoved(qulonglong id);

private:
        NfCore::NfUserTaskManager* m_taskManager;
        NfCore::NfUserTaskManager::SubscriptionId m_subscription;
};

} // namespace NfUi

#endif // NF_USER_TASKS_ADAPTER_H
