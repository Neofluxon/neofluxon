/**
 * File name: NfProjectsAdapter.h
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

#include "NfProjectsAdapter.h"
#include "core/NfProjectsManager.h"
#include "core/NfProjectData.h"

#include <utility>
#include <vector>

using namespace NfCore;

namespace NfUi {

NfProjectsAdapter::NfProjectsAdapter(NfProjectsManager* taskManager,
                                       QObject* parent)
        : QObject(parent)
        , m_taskManager{taskManager}
{
        m_subscription = m_taskManager->subscribe({
                        .projectListUpdated = [this](std::vector<NfProjectData> data) {
                                emit projectListUpdated(std::move(data));
                        },
                        .projectAdded = [this](NfProjectData data) {
                                emit projectAdded(id, std::move(data));
                        },
                        .projectUpdated = [this](NfProjectData data) {
                                emit projectUpdated(id, std::move(data));
                        },
                        .projectRemoved = [this](int64_t id) {
                                emit projectRemoved(id);
                        }
        });
}

NfProjectsAdapter::~NfProjectsAdapter()
{
        m_taskManager->unsubscribe(m_subscription);
}

} // namespace NfUi
