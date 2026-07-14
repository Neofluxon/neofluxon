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
#include "NfSourceRecords.h"
#include "NfLibraryDatabase.h"
#include "NfLibrary.h"
#include "NfLibraryFolderImportTask.h"
#include "NfScheduler.h"
#include "NfLibraryQuery.h"
#include "NfLogger.h"

namespace NfCore {

NfLibraryManager::NfLibraryManager(NfScheduler *scheduler)
        : m_scheduler{scheduler}
        , m_database{std::make_unique<NfLibraryDatabase>("./neofluxon.db")}
{
        if (!m_database->initializeSchema()) {
                NF_LOG_ERROR("can't initialize schema for DB: " << m_database->path() );
        }
}

NfLibraryManager::~NfLibraryManager()
{
}

std::vector<std::unique_ptr<NfLibrary>>
NfLibraryManager::libraries() const
{
        std::vector<std::unique_ptr<NfLibrary>> libraries;

        auto librariesIds = m_database->libraryIds();
        for (const auto &id: librariesIds)
                libraries.push_back(std::make_unique<NfLibrary>(m_database.get(), id));

        return libraries;
}

std::optional<std::unique_ptr<NfLibrary>>
NfLibraryManager::addLibrary(std::string_view name)
{
        NfLibraryDatabase::Transaction tx(m_database.get());

        if (auto record = m_database->library(name)) {
                tx.commit();
                return std::make_unique<NfLibrary>(m_database.get(), record->id);
        }

        const auto id = m_database->addLibrary(name);
        if (id < 0)
                return nullptr;

        tx.commit();

        return std::make_unique<NfLibrary>(m_database.get(), id);
}

std::unique_ptr<NfLibrary> NfLibraryManager::getLibrary(uint64_t id) const
{
        using Transaction = NfLibraryDatabase::Transaction;

        {
                NfLibraryDatabase::Transaction tx(m_database.get(),
                                                  Transaction::Mode::LockOnly);
                if (!m_database->libraryExists(id))
                        return nullptr;
        }

        return std::make_unique<NfLibrary>(m_database.get(), id);
}

void NfLibraryManager::importPath(const std::filesystem::path& path, uint64_t id)
{
        NF_LOG_DEBUG("import path: " << path);

        auto library = getLibrary(id);
        if (!library) {
                NF_LOG_ERROR("library doesn't exist: id = " << id);
                return;
        }

        auto task = std::make_unique<NfLibraryFolderImportTask>(path, std::move(library));
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* importTask = dynamic_cast<NfLibraryFolderImportTask*>(result);
                if (importTask) {
                        NF_LOG_DEBUG("library import finished");
                }
                });

        NF_LOG_DEBUG("import task submitted for : " << path);

        m_scheduler->submit(std::move(task));
}

std::vector<NfPhoto>
NfLibraryManager::queryPhotos(const NfLibraryQuery& query) const
{
        if (auto library = getLibrary(query.libraryId))
                return library->queryPhotos(query);

        const auto allLibraries = libraries();
        std::vector<NfPhoto> result;
        for (const auto& lib : allLibraries) {
                auto photos = lib->queryPhotos(query);
                result.insert(result.end(),
                              std::make_move_iterator(photos.begin()),
                              std::make_move_iterator(photos.end()));
        }

        return result;
}

} // namespace NfCore
