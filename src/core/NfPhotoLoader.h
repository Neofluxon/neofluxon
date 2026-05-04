/**
 * File name: NfPhotoLoader.h
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

#ifndef NF_PHOTO_LOADER_H
#define NF_PHOTO_LOADER_H

#include "NfPhoto.h"
#include "NfPhotoId.h"
#include "NfForegroundThreadPool.h"
#include "NfThumbnail.h"
#include "NfPreview.h"
#include "NfTask.h"

#include <filesystem>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>
#include <unordered_set>

namespace NfCore {

class NfPathScanner;
class NfImage;
class NfCache;

class NfPhotoLoader {
public:
        enum RequestType {
                Visible,
                Prefetch
        };

        NfPhotoLoader(NfCache *thumbnailsCache, NfCache *previewsCache);
        ~NfPhotoLoader();

        void setPath(const std::filesystem::path &path);
        const std::filesystem::path& getPath() const;

        void requestThumbnail(const NfPhoto &photo,
                              RequestType requestType = RequestType::Visible);
        void requestPreview(const NfPhoto &photo,
                            RequestType requestType = RequestType::Visible);

        std::vector<NfPhoto> takePhotos();
        std::vector<NfPhotoId> takeThumbnails();
        std::vector<NfPhotoId> takePreviews();

 protected:

 private:
        static NfTask::Priority requestTypeToPriority(RequestType type);

        std::unique_ptr<NfPathScanner> m_pathScanner;
        std::filesystem::path m_path;
        NfForegroundThreadPool m_threadPool;
        NfCache* m_thumbnailsCache;
        NfCache* m_previewsCache;
        std::mutex m_mutex;
        uint64_t m_generationId;
        std::vector<NfPhotoId> m_thumbnailsQueue;
        std::vector<NfPhotoId> m_previewsQueue;
        uint64_t m_sequence = 0;
};

} // namespace NfCore

#endif // NF_PHOTO_LOADER_H
