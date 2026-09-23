/**
 * File name: NfPhotoMetadataLoader.h
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

#ifndef NF_PHOTO_METADATA_LOADER_H
#define NF_PHOTO_METADATA_LOADER_H

#include "NfPhoto.h"
#include "NfPhotoId.h"
#include "NfPhotoMetadata.h"
#include "NfPhotoMetadataTask.h"
#include "NfTask.h"
#include "NfPhotoSource.h"
#include "NfPhotoLoaderContext.h"
#include "NfRequest.h"

#include <vector>
#include <cstdint>
#include <mutex>
#include <utility>
#include <unordered_set>

namespace NfCore {

class NfScheduler;
class NfImage;
class NfCache;

using NfPhotoMetadataLoaderContext = NfPhotoLoaderContext;

class NfPhotoMetadataLoader {
public:
        NfPhotoMetadataLoader(NfPhotoMetadataLoaderContext ctx);
        ~NfPhotoMetadataLoader();
        void requestMetadata(const NfPhoto &photo,
                             NfRequestType request = NfRequestType::SelectedMetadata);
        std::vector<std::pair<NfPhotoId, NfPhotoMetadata>> takeMetadata();

 protected:

 private:
         NfPhotoMetadataLoaderContext m_context;
         NfScheduler* m_scheduler;
         std::mutex m_mutex;
         std::vector<std::pair<NfPhotoId, NfPhotoMetadata>> m_metadataQueue;
         std::unordered_set<NfPhotoId> m_pendingRequests;
 };

} // namespace NfCore

#endif // NF_PHOTO_METADATA_LOADER_H
