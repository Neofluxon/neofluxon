/**
 * File name: NfLibrary.cpp
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

#include "NfLibrary.h"
#include "NfSourceRecords.h"
#include "NfLibraryRepresentation.h"
#include "NfLibraryDatabase.h"
#include "NfPhoto.h"
#include "NfPhotoMetadataExtractor.h"
#include "NfLogger.h"

#include <algorithm>

namespace NfCore {

NfLibrary::NfLibrary(NfLibraryDatabase *db, uint64_t id)
        : m_db{db}
        , m_id{id}
{
        //auto rec = m_db->getLibraryRecord(id);
        //m_name = rec->name;
        //for (const auto& repId: rec->representationsIds())
        //        m_representations.push_back(std::make_unique<NfRepresentation>(db, repId));
}

NfLibrary::~NfLibrary()
{
}

uint64_t NfLibrary::id() const
{
        return m_id;
}

void NfLibrary::setName(std::string_view name)
{
        //m_name = name;
}

std::string NfLibrary::name() const noexcept
{
        using Mode = NfLibraryDatabase::Transaction::Mode;
        NfLibraryDatabase::Transaction tx(m_db, Mode::LockOnly);
        return m_db->library(m_id)->name;
}

NfLibraryRepresentation* NfLibrary::addRepresentation()
{
        /*auto representation = std::make_unique<NfLibraryRepresentation>();
        auto* ptr = representation.get();
        m_representations.push_back(std::move(representation));

        return nullptr ptr;*/
        return nullptr;
}

void NfLibrary::removeRepresentation(NfLibraryRepresentation* representation)
{
        /*auto it = std::remove_if(m_representations.begin(),
                                 m_representations.end(),
                                 [representation](const auto& item) {
                                         return item.get() == representation;
        });

        m_representations.erase(it, m_representations.end());*/
}

std::vector<std::unique_ptr<NfLibraryRepresentation>>NfLibrary::representations() const
{
        std::vector<std::unique_ptr<NfLibraryRepresentation>> reps;
        using RT = NfLibraryRepresentation::RepresentationType;

        reps.push_back(std::make_unique<NfLibraryRepresentation>(m_db, m_id, RT::DateTime));
        reps.push_back(std::make_unique<NfLibraryRepresentation>(m_db, m_id, RT::Canonical));
        reps.push_back(std::make_unique<NfLibraryRepresentation>(m_db, m_id, RT::Equipment));
        reps.push_back(std::make_unique<NfLibraryRepresentation>(m_db, m_id, RT::Collections));

        return reps;
}

void NfLibrary::addPhoto(const NfPhoto& photo)
{
        NF_LOG_DEBUG("add photo: " << photo.path());

        auto info = NfPhotoMetadataExtractor(photo).summaryInfo();

        NfLibraryDatabase::Transaction tx(m_db);

        auto folderId = m_db->addFolder(photo.path().parent_path(), id());
        if (folderId < 0) {
                NF_LOG_ERROR("Failed to add folder: " << photo.path());
                return;
        }

        NF_LOG_DEBUG("folder id: " << folderId);

        int cameraId = storeCamera(info.cameraMaker, info.cameraModel);
        int lensId   = storeLens(info.lens);

        if (cameraId == -2 || lensId == -2)
                return;

        auto id = m_db->addImage(folderId,
                                 photo.name(),
                                 info.dateTaken.time_since_epoch().count(),
                                 cameraId,
                                 lensId);
        if (id < 0) {
                NF_LOG_ERROR("Failed to add image entry: " << photo.name());
                return;
        }

        tx.commit();
}

std::vector<NfPhoto> NfLibrary::queryPhotos(const NfLibraryQuery &query) const
{
        using RepresentationType = NfLibraryRepresentation::RepresentationType;

        std::vector<NfPhoto> result;

        switch (query.representationType) {
        case RepresentationType::DateTime:
        case RepresentationType::Canonical:
        case RepresentationType::Equipment:
        case RepresentationType::Collections:
                break;
        default:
                type = RepresentationType::Canonical;
                break;
        }

        auto rep = std::make_unique<NfLibraryRepresentation>(m_db, m_id, type);

        auto photos = rep->queryPhotos(query);
        result.append_range(std::move(photos));

        return result;
}

int64_t NfLibrary::storeCamera(std::string_view maker,
                               std::string_view model)
{
        if (maker.empty())
                return -1;

        auto cameraId = m_db->addCamera(maker, model);
        if (cameraId < 0) {
                NF_LOG_ERROR("Database error writing camera details: "
                             << maker << " " << model);
                return -2;
        }

        return cameraId;
}

int64_t NfLibrary::storeLens(std::string_view lensName)
{
        if (lensName.empty())
                return -1;

        auto lensId = m_db->addLens(lensName);
        if (lensId < 0) {
                NF_LOG_ERROR("Database error writing lens details: " << lensName);
                return -2;
        }

        return lensId;
}

} // namespace NfCore
