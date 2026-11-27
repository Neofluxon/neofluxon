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
#include "NfLibraryTreeTask.h"
#include "NfSourceRecords.h"
#include "NfDatabase.h"
#include "NfLibrary.h"
#include "NfImportFolderTask.h"
#include "NfUserTaskManager.h"
#include "NfScheduler.h"
#include "NfLibraryQuery.h"
#include "NfLogger.h"
#include "NfFileUtils.h"

#include <exception>

namespace NfCore {

NfLibraryManager::NfLibraryManager(NfDatabase* db,
                                   NfScheduler *backgroundScheduler,
                                   NfScheduler *foregroundScheduler,
                                   NfUserTaskManager* userTaskManager)
        : m_db{db}
        , m_backgroundScheduler{backgroundScheduler}
        , m_foregroundScheduler{foregroundScheduler}
        , m_userTaskManager{userTaskManager}
{
}

NfLibraryManager::~NfLibraryManager()
{
}

std::vector<std::unique_ptr<NfLibrary>>
NfLibraryManager::libraries() const
{
        using Transaction = NfDatabase::Transaction;
        std::vector<int64_t> librariesIds;
        {
                Transaction tx(m_db, Transaction::Mode::LockOnly);
                librariesIds = m_db->libraryIds();
        }

        std::vector<std::unique_ptr<NfLibrary>> libraries;
        for (const auto &id: librariesIds)
                libraries.push_back(std::make_unique<NfLibrary>(m_db, id));

        return libraries;
}

std::optional<std::unique_ptr<NfLibrary>>
NfLibraryManager::addLibrary(std::string_view name)
{
        using Transaction = NfDatabase::Transaction;
        {
                Transaction tx(m_db, Transaction::Mode::LockOnly);
                if (auto record = m_db->library(name))
                        return std::make_unique<NfLibrary>(m_db, record->id);
        }

        Transaction tx(m_db);
        const auto id = m_db->addLibrary(name);
        if (id < 0)
                return nullptr;

        tx.commit();

        return std::make_unique<NfLibrary>(m_db, id);
}

bool NfLibraryManager::updateLibrary(uint64_t libraryId, std::string_view name)
{
        using Transaction = NfDatabase::Transaction;
        Transaction tx(m_db);
        if (!m_db->updateLibrary(static_cast<int64_t>(libraryId), name))
                return false;

        tx.commit();
        return true;
}

bool NfLibraryManager::removeLibrary(uint64_t libraryId)
{
        using Transaction = NfDatabase::Transaction;
        Transaction tx(m_db);
        auto res = m_db->removeLibrary(static_cast<int64_t>(libraryId));
        if (!res)
                return res;

        tx.commit();

        return true;
}

std::unique_ptr<NfLibrary> NfLibraryManager::getLibrary(uint64_t id) const
{
        using Transaction = NfDatabase::Transaction;
        {
                NfDatabase::Transaction tx(m_db,
                                           Transaction::Mode::LockOnly);
                if (!m_db->libraryExists(id))
                        return nullptr;
        }

        return std::make_unique<NfLibrary>(m_db, id);
}

void NfLibraryManager::importPath(const std::filesystem::path& path, uint64_t id)
{
        auto library = getLibrary(id);
        if (!library) {
                NF_LOG_ERROR("library doesn't exist: id = " << id);
                return;
        }

        auto task = std::make_unique<NfImportFolderTask>(path,
                                                         std::move(library),
                                                         m_userTaskManager);
        task->setResult([this](NfTask* result, NfTask::TaskStatus status) {
                auto* importTask = dynamic_cast<NfImportFolderTask*>(result);
                if (!importTask) {
                        NF_LOG_ERROR("not an import folder task");
                        return;
                }

                if (status != NfTask::TaskStatus::Success) {
                        NF_LOG_ERROR("import folder task failed for:  "
                                     << importTask->path());
                        importTask->notifyRemoved();
                        return;
                }

                std::scoped_lock lock(m_mutex);
                if (m_folderImportedCb)
                        m_folderImportedCb();

                importTask->notifyRemoved();

                NF_LOG_DEBUG("library import finished for: "
                             << importTask->path());
        });

        task->notifySubmitted();
        m_backgroundScheduler->submit(std::move(task));

        NF_LOG_DEBUG("import task submitted for : " << path);
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

void NfLibraryManager::loadTreeAsync(LibraryTreeResultCallback resultCb,
                                     NfRequestType request)
{
        auto task = std::make_unique<NfLibraryTreeTask>(this);
        task->setPriority(NfRequestUtils::getTaskPriority(request));
        task->setResult([resultCb = std::move(resultCb)](NfTask* result,
                                                         NfTask::TaskStatus status) mutable {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* treeTask = dynamic_cast<NfLibraryTreeTask*>(result);
                if (!treeTask)
                        return;

                resultCb(treeTask->takeSnapshot());
        });

        m_foregroundScheduler->submit(std::move(task));
}

void NfLibraryManager::loadLibraryBranchAsync(int64_t libraryId,
                                              LibraryResultCallback resultCb,
                                              NfRequestType request)
{
        auto task = std::make_unique<NfLibraryTreeTask>(this, libraryId);
        task->setPriority(NfRequestUtils::getTaskPriority(request));
        task->setResult([resultCb = std::move(resultCb)](NfTask* result,
                                                         NfTask::TaskStatus status) mutable {
                if (status != NfTask::TaskStatus::Success)
                        return;

                auto* treeTask = dynamic_cast<NfLibraryTreeTask*>(result);
                if (!treeTask)
                        return;

                auto snapshot = treeTask->takeSnapshot();
                if (!snapshot.empty())
                        resultCb(std::move(snapshot.front()));
        });

        m_foregroundScheduler->submit(std::move(task));
}

void NfLibraryManager::setFolderImportedCallback(FolderImportedCallback cb)
{
        m_folderImportedCb = std::move(cb);
}

bool NfLibraryManager::setImageFavorite(int64_t libraryId,
                                        const NfPhotoId& photoId,
                                        bool favorite)
{
        auto library = getLibrary(libraryId);
        if (library)
                return library->setImageFavorite(photoId, favorite);

        return false;
}

bool NfLibraryManager::setImageRating(int64_t libraryId,
                                      const NfPhotoId& photoId,
                                      int rating)
{
        auto library = getLibrary(libraryId);
        if (library)
                return library->setImageRating(photoId, rating);

        return false;
}

} // namespace NfCore
