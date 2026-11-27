/**
 * File name: NfProjectsManager.cpp
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

#include "NfProjectsManager.h"
#include "NfDatabase.h"
#include "NfLogger.h"

namespace NfCore {

NfProjectsManager::NfProjectsManager(NfDatabase *db)
        : m_db{db}
{
}

std::vector<NfProjectData> NfProjectsManager::getProjects() const
{
        using Mode = NfDatabase::Transaction::Mode;
        NfDatabase::Transaction tx(m_db, Mode::LockOnly);
        return m_db->getProjects();
}

std::optional<NfProjectData> NfProjectsManager::addProject(NfCore::NfProjectInfo info)
{
        NfDatabase::Transaction tx(m_db);
        auto project = m_db->addProject(std::move(info));
        if (!project || project->id < 0) {
                NF_LOG_ERROR("can't add project: " << info.name);
                return std::nullopt;
        }

        tx.commit();

        return project;
}

std::optional<NfProjectData> NfProjectsManager::updateProject(uint64_t id, NfCore::NfProjectInfo info)
{
        NfDatabase::Transaction tx(m_db);
        auto project = m_db->updateProject(id, std::move(info));
        if (!project || project->id < 0) {
                NF_LOG_ERROR("can't update project [id = " << id << "]: " << info.name);
                return std::nullopt;
        }

        tx.commit();

        return project;
}

bool NfProjectsManager::removeProject(uint64_t id)
{
        return m_db->removeProject(id);
}

} // namespace NfCore

