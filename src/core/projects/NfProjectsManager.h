/**
 * File name: NfProjectsManager.h
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

#ifndef NF_PROJECTS_MANAGER_H
#define NF_PROJECTS_MANAGER_H

#include "NfProjectInfo.h"
#include "NfProjectData.h"

#include <vector>
#include <optional>

namespace NfCore {

class NfDatabase;

/**
* TODO: For now the projects manger makes synchronous requests to DB.
* There shuold not be too many projects, and if this creates UI
* blocking than move to async requestes to DB.
*/
class NfProjectsManager  {
public:
        explicit NfProjectsManager(NfDatabase *db);
        NfProjectsManager(const NfProjectsManager&) = delete;
        NfProjectsManager& operator=(const NfProjectsManager&) = delete;
        std::vector<NfProjectData> getProjects() const;
        std::optional<NfProjectData> addProject(NfCore::NfProjectInfo info);
        std::optional<NfProjectData> updateProject(uint64_t id, NfCore::NfProjectInfo info);
        bool removeProject(uint64_t id);

private:
        NfDatabase *m_db;
};

} // namespace NfCore

#endif // NF_PROJECTS_MANAGER_H
