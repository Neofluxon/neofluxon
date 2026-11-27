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

using namespace NfCore;

namespace NfUi {

NfProjectsAdapter::NfProjectsAdapter(NfProjectsManager* taskManager,
                                     QObject* parent)
        : QObject(parent)
        , m_projectsManager{taskManager}
{
}

NfProjectsAdapter::~NfProjectsAdapter()
{
}

void NfProjectsAdapter::requestProjects()
{
        // TODO: there is not supposed to be too many projects, thus,
        // for now blocking request to DB will be used.
        emit projectsListUpdated(m_projectsManager->getProjects());

}

void NfProjectsAdapter::addProject(NfProjectInfo info)
{
        auto project = m_projectsManager->addProject(std::move(info));
        if (project && project->id >= 0)
                emit projectAdded(std::move(*project));

}

void NfProjectsAdapter::updateProject(uint64_t id, NfProjectInfo info)
{
        auto project = m_projectsManager->updateProject(id, std::move(info));
        if (project && project->id >= 0)
                emit projectUpdated(std::move(*project));

}

void NfProjectsAdapter::removeProject(uint64_t id)
{
        if (m_projectsManager->removeProject(id))
                emit projectRemoved(id);

}

} // namespace NfUi
