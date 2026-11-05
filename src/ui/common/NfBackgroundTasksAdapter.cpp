/**
 * File name: NfBackgroundTasksAdapter.cpp
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

#include "NfBackgroundTasksAdapter.h"

namespace NfUi {

NfBackgroundTasksAdapter::NfBackgroundTasksAdapter(QObject* parent)
        : QObject(parent)
        , m_tasks{
                {QStringLiteral("mock-folder-import-1"),
                 tr("Import folder \"/home/user/folder1\" to library"),
                 QStringLiteral("/home/user/folder1"),
                 NfBackgroundTask::State::Running,
                 42,
                 tr("1,240 / 2,950 photos"),
                 tr("Cancel")}}
{
}

const std::vector<NfBackgroundTask>& NfBackgroundTasksAdapter::tasks() const
{
        return m_tasks;
}

void NfBackgroundTasksAdapter::requestCancelTask(const QString& taskId)
{
        emit cancelTaskRequested(taskId);
}

void NfBackgroundTasksAdapter::requestPauseTask(const QString& taskId)
{
        emit pauseTaskRequested(taskId);
}

void NfBackgroundTasksAdapter::requestResumeTask(const QString& taskId)
{
        emit resumeTaskRequested(taskId);
}

void NfBackgroundTasksAdapter::requestRestartTask(const QString& taskId)
{
        emit restartTaskRequested(taskId);
}

} // namespace NfUi
