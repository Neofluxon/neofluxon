/**
 * File name: NfPathScanner.h
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

#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <filesystem>
#include <stop_token>
#include <condition_variable>
#include <unordered_set>

namespace NfCore {

class NfPathScanner {
public:
        NfPathScanner();
        ~NfPathScanner();

        void setPath(const std::filesystem::path& path, bool recursive = true);
        std::filesystem::path getPath(const std::filesystem::path& path) const;
        std::vector<NfPhoto> takePhotos();

protected:
        void loadPhotosThread(std::stop_token stopToken);
        void processPathEntry(const std::filesystem::path& path);

private:
        mutable std::mutex m_mutex;
        std::string m_path;
        bool m_recursive;
        std::vector<NfPhoto> m_loadedPhotos;

        std::jthread m_scanThread;
        std::atomic<bool> m_startScan;
        std::condition_variable_any m_conditionVariable;
        std::unordered_set<std::string> m_photoExtentions;
};

} // namepsace NfCore

#endif // NF_PHOTO_SCANNER_H
