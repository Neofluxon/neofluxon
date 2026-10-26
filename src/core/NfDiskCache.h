/**
 * File name: NfDiskCache.h
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef NF_DISK_CACHE_H
#define NF_DISK_CACHE_H

#include "NfDiskCacheSettings.h"
#include "NfImage.h"
#include "NfPhotoId.h"

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>

namespace NfCore {

class NfDiskCache {
public:
        enum class CacheType {
                Thumbnail,
                Preview
        };

        explicit NfDiskCache(NfDiskCacheSettings settings);
        ~NfDiskCache();
        NfDiskCache(const NfDiskCache&) = delete;
        NfDiskCache& operator=(const NfDiskCache&) = delete;
        void enqueue(CacheType type, const NfPhotoId& id, std::unique_ptr<NfImage> image);
        std::unique_ptr<NfImage> get(CacheType type, const NfPhotoId& id);
        bool put(CacheType type, const NfPhotoId& id, const NfImage& image);

private:
        struct WriteRequest {
                CacheType type;
                NfPhotoId id;
                std::unique_ptr<NfImage> image;
        };

        std::filesystem::path entryPath(CacheType type, const NfPhotoId& id) const;
        bool evictToFit(std::uint64_t requiredBytes,
                        const std::filesystem::path& protectedPath);
        void workerLoop();

        NfDiskCacheSettings m_settings;
        std::mutex m_mutex;
        std::mutex m_queueMutex;
        std::condition_variable m_queueCondition;
        std::deque<WriteRequest> m_writeQueue;
        bool m_stopping{false};
        std::thread m_worker;
};

} // namespace NfCore

#endif // NF_DISK_CACHE_H
