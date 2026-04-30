/**
 * File name: NfPathScanner.cpp
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

#include "NfPathScanner.h"
#include "NfLogger.h"

namespace NfCore {

NfPathScanner::NfPathScanner()
        : m_recursive{true}
        , m_startScan{false}
        , m_photoExtentions{".cr3", ".cr2", ".dng", ".raf", ".nef", ".jpg", ".jpeg", ".png"}
{
        m_scanThread = std::jthread([this](std::stop_token token) {
                loadPhotosThread(token);
        });
}

NfPathScanner::~NfPathScanner()
{
        NF_LOG_DEBUG("called");

        m_scanThread.request_stop();
        m_conditionVariable.notify_one();
}

void NfPathScanner::setPath(const std::filesystem::path& path, bool recursive)
{
        {
                std::lock_guard lock(m_mutex);
                m_path = path;
                m_recursive = recursive;
                m_startScan.store(true, std::memory_order_relaxed);
                m_loadedPhotos.clear();
        }

        m_conditionVariable.notify_one();

        NF_LOG_DEBUG("path: " << path);
}

void NfPathScanner::loadPhotosThread(std::stop_token stopToken)
{
        namespace fs = std::filesystem;

        NF_LOG_DEBUG("start thread");

        while (!stopToken.stop_requested()) {
                std::string directory;
                bool recursive;

                {
                        std::unique_lock lock(m_mutex);

                        NF_LOG_DEBUG("wait...");
                        m_conditionVariable.wait(lock, stopToken, [&]() {
                                return m_startScan.load(std::memory_order_relaxed);
                        });

                        NF_LOG_DEBUG("thread wakeup...");

                        m_loadedPhotos.clear();

                        if (stopToken.stop_requested())
                                break;

                        directory = m_path;
                        recursive = m_recursive;
                        m_startScan.store(false, std::memory_order_relaxed);
                }

                try {
                        if (recursive) {
                                fs::recursive_directory_iterator it(directory), end;

                                for (; it != end; ++it) {
                                        if (stopToken.stop_requested())
                                                return;

                                        if (m_startScan.load(std::memory_order_relaxed))
                                                break;

                                        processPathEntry(it->path());
                                }
                        } else {

                                NF_LOG_DEBUG("iterate dir: " << directory);

                                fs::directory_iterator it(directory), end;

                                for (; it != end; ++it) {
                                        if (stopToken.stop_requested())
                                                return;

                                        if (m_startScan.load(std::memory_order_relaxed))
                                                break;

                                        processPathEntry(it->path());
                                }
                        }
                }
                catch (const std::filesystem::filesystem_error&) {
                }
        }

        NF_LOG_DEBUG("exit thread");
}

void NfPathScanner::processPathEntry(const std::filesystem::path& path)
{
        //NF_LOG_DEBUG("path: " << path);

        namespace fs = std::filesystem;

        try {
                if (!fs::is_regular_file(path))
                        return;

                std::string ext = path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](unsigned char c) { return std::tolower(c); });

                if (!m_photoExtentions.contains(ext))
                        return;

                NfPhoto photo(path);
                {
                        std::lock_guard lock(m_mutex);
                        NF_LOG_DEBUG("NEW PHOTO: " << photo.path());
                        m_loadedPhotos.push_back(std::move(photo));
                }
        }
        catch (const fs::filesystem_error&) {
        }
}

std::vector<NfPhoto> NfPathScanner::takePhotos()
{
        std::lock_guard lock(m_mutex);
        return std::move(m_loadedPhotos);
}

} // namespace NfCore
