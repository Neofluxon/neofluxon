/**
 * File name: NfPhotoScanner.h
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

#ifndef NF_PHOTO_SCANNER_H
#define NF_PHOTO_SCANNER_H

#include "NfPhoto.h"
#include "NfPhotoSource.h"

#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>

namespace NfCore {

class NfTask;
class NfScheduler;
class NfThreadPool;
class NfFileSystemPhotoSource;
class NfLibraryPhotoSource;
class NfLibraryManager;

class NfPhotoScanner {
public:
        NfPhotoScanner(NfLibraryManager *library);
        ~NfPhotoScanner();
        void setSource(const NfPhotoSource &source);
        std::vector<NfPhoto> takePhotos();

private:
        std::unique_ptr<NfTask> createTask(const NfFilesystemPhotoSource& source);
        std::unique_ptr<NfTask> createTask(const NfLibraryPhotoSource& source);

        NfLibraryManager* m_library;
        std::unique_ptr<NfScheduler> m_scheduler;
        std::unique_ptr<NfThreadPool> m_threadPool;
        mutable std::mutex m_mutex;
        uint64_t m_generationId{0};
        std::vector<NfPhoto> m_loadedPhotos;
        std::unordered_set<std::string> m_photoExtentions;
};

} // namepsace NfCore

#endif // NF_PHOTO_SCANNER_H
