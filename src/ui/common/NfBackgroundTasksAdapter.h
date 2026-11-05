/**
 * File name: NfBackgroundTasksAdapter.h
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

#ifndef NF_BACKGROUND_TASKS_ADAPTER_H
#define NF_BACKGROUND_TASKS_ADAPTER_H

#include "NfBackgroundTask.h"

#include <QObject>

#include <vector>

namespace NfUi {

class NfBackgroundTasksAdapter : public QObject
{
        Q_OBJECT

public:
        explicit NfBackgroundTasksAdapter(QObject* parent = nullptr);

        const std::vector<NfBackgroundTask>& tasks() const;
        void requestCancelTask(const QString& taskId);
        void requestPauseTask(const QString& taskId);
        void requestResumeTask(const QString& taskId);
        void requestRestartTask(const QString& taskId);

signals:
        void taskAdded(const NfUi::NfBackgroundTask& task);
        void taskUpdated(const NfUi::NfBackgroundTask& task);
        void taskRemoved(const QString& taskId);
        void cancelTaskRequested(const QString& taskId);
        void pauseTaskRequested(const QString& taskId);
        void resumeTaskRequested(const QString& taskId);
        void restartTaskRequested(const QString& taskId);

private:
        std::vector<NfBackgroundTask> m_tasks;
};

} // namespace NfUi

#endif // NF_BACKGROUND_TASKS_ADAPTER_H
