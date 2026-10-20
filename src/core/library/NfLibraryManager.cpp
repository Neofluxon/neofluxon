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
#include "NfApplicationPaths.h"
#include "NfSourceRecords.h"
#include "NfLibraryDatabase.h"
#include "NfLibrary.h"
#include "NfLibraryFolderImportTask.h"
#include "NfScheduler.h"
#include "NfLibraryQuery.h"
#include "NfLogger.h"
#include "NfFileUtils.h"

#include <exception>

namespace NfCore {

NfLibraryManager::NfLibraryManager(NfScheduler *scheduler)
        : m_scheduler{scheduler}
{
}

NfLibraryManager::~NfLibraryManager()
{
}

bool NfLibraryManager::initialize(std::string& errorMessage)
{
        NF_LOG_DEBUG("called");

        if (m_database)
                return true;

        try {
                const auto databasePath = NfApplicationPaths().dataDirectory() / "neofluxon.db";
                auto database = std::make_unique<NfLibraryDatabase>(databasePath);
                if (!database->open(errorMessage)) {
                        errorMessage = "Unable to open library database '"
                                + NfFileUtils::pathToUtf8(databasePath) + "': " + errorMessage;
                        NF_LOG_ERROR(errorMessage);
                        return false;
                }

                if (!database->initializeSchema(errorMessage)) {
                        errorMessage = "Unable to initialize library database schema '"
                                + NfFileUtils::pathToUtf8(databasePath) + "': " + errorMessage;
                        NF_LOG_ERROR(errorMessage);
                        return false;
                }

                m_database = std::move(database);

                NF_LOG_DEBUG("Database created: " << databasePath);
        } catch (const std::exception& exception) {
                errorMessage = "Unable to initialize library database: ";
                errorMessage += exception.what();
                NF_LOG_ERROR(errorMessage);
                return false;
        }

        errorMessage.clear();

        return true;
}

std::vector<std::unique_ptr<NfLibrary>>
NfLibraryManager::libraries() const
{
        using Transaction = NfLibraryDatabase::Transaction;
        std::vector<int64_t> librariesIds;
        {
                Transaction tx(m_database.get(), Transaction::Mode::LockOnly);
                librariesIds = m_database->libraryIds();
        }

        std::vector<std::unique_ptr<NfLibrary>> libraries;
        for (const auto &id: librariesIds)
                libraries.push_back(std::make_unique<NfLibrary>(m_database.get(), id));

        return libraries;
}

std::optional<std::unique_ptr<NfLibrary>>
NfLibraryManager::addLibrary(std::string_view name)
{
        using Transaction = NfLibraryDatabase::Transaction;
        {
                Transaction tx(m_database.get(), Transaction::Mode::LockOnly);
                if (auto record = m_database->library(name))
                        return std::make_unique<NfLibrary>(m_database.get(), record->id);
        }

        Transaction tx(m_database.get());
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
                        std::scoped_lock lock(m_mutex);
                        if (m_folderImportedCb)
                                m_folderImportedCb();

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

void NfLibraryManager::setFolderImportedCallback(FolderImportedCallback cb)
{
        m_folderImportedCb = std::move(cb);
}

} // namespace NfCore
