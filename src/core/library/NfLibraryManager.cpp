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
#include "NfLibraryDatabase.h"
#include "NfLibrary.h"
#include "NfLibraryFolderImportTask.h"
#include "NfScheduler.h"
#include "NfLogger.h"

namespace NfCore {

NfLibraryManager::NfLibraryManager(NfScheduler *scheduler)
        : m_scheduler{scheduler}
        , m_database{std::make_unique<NfLibraryDatabase>("./neofluxon.db")}
{
        if (!m_database->initializeSchema()) {
                NF_LOG_ERROR("can't initialize schema for DB: " << m_database->path() );
        }

        for (const auto& id : m_database->libraries())
                m_libraries.push_back(std::make_unique<NfLibrary>(m_database.get(), id));
}

NfLibraryManager::~NfLibraryManager()
{
}

const std::vector<std::unique_ptr<NfLibrary>>&
NfLibraryManager::libraries() const
{
        return m_libraries;
}

NfLibrary* NfLibraryManager::addLibrary(std::string_view name)
{
        if (auto* library = getLibraryByName(name))
                return library;

        std::scoped_lock lock(m_mutex);

        const auto id = m_database->addLibrary(name);
        if (id < 0)
                return nullptr;

        auto& library = m_libraries.push_back(std::make_unique<NfLibrary>(m_database.get(), id));

        return library.get();
}

NfLibrary* NfLibraryManager::getLibrary(uint64_t id) const
{
        std::scoped_lock lock(m_mutex);
        auto it = std::find_if(m_libraries.begin(),
                               m_libraries.end(),
                               [id](const auto & library) {
                                       return library->id() == id;
                               });

        return (it != m_libraries.end()) ? it->get() : nullptr;
}

NfLibrary* NfLibraryManager::getLibraryByName(std::string_view name) const
{
        std::scoped_lock lock(m_mutex);
        auto it = std::find_if(m_libraries.begin(),
                               m_libraries.end(),
                               [name](const auto& library) {
                                       return library->name() == name;
                               });

        return (it != m_libraries.end()) ? it->get() : nullptr;
}

void NfLibraryManager::importPath(const std::filesystem::path& path, uint64_t id)
{
        NF_LOG_DEBUG("import path: " << path);

        auto library = getLibrary(id);
        if (!library)
                return;

        std::scoped_lock lock(m_mutex);
        auto task = std::make_unique<NfLibraryFolderImportTask>(path, library);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* importTask = dynamic_cast<NfLibraryFolderImportTask*>(result);
                if (importTask) {
                        NF_LOG_DEBUG("library import finished");
                        // TOTO...
                }
                });

        NF_LOG_DEBUG("import task submitted for : " << path);
        m_scheduler->submit(std::move(task));
}

} // namespace NfCore
