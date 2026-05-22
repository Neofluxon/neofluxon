/**
 * File name: NfLibraryManager.cpp
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

#include "NfLibraryManager.h"
#include "NfLibrary.h"

namespace NfCore {

NfLibraryManager::NfLibraryManager()
        : m_database(std::make_unique<NfLibraryDatabase>())
{
        for (const auto& id : m_database->libraries())
                m_libraries.push_back(std::make_unique<NfLibrary>(m_database, id));
}

NfLibraryManager::~NfLibraryManager()
{
}

const std::vector<std::unique_ptr<NfLibrary>>&
NfLibraryManager::libraries() const
{
        return m_libraries;
}

NfLibrary* NfLibraryManager::getLibrary(NfLibraryId id) const
{
}

void NfLibraryManager::importPath(const std::filesystem::path& path, NfLibraryId id)
{
        std::scoped_lock lock(m_mutex);
        auto library = getLibrary(id);
        if (!library)
                return;

        auto task = std::make_unique<NfLibraryImportTask>(path, library);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* importTask = dynamic_cast<NfLibraryImportTask*>(result);
                if (importTask) {
                        // TOTO...
                }
                });

        m_scheduler->submit(std::move(task));
}

} // namespace NfCore
