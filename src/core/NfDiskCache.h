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
#include <cstdint>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <map>
#include <unordered_set>
#include <vector>

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

protected:

private:
        struct WriteRequest {
                CacheType type;
                NfPhotoId id;
                std::unique_ptr<NfImage> image;
        };

        struct CacheEntry {
                CacheType type;
                std::uint64_t id;
                std::uint64_t size;
        };

        using IndexedEntry =
                std::pair<CacheEntry, std::filesystem::file_time_type>;

        void indexEntries(CacheType type, std::vector<IndexedEntry>& entries);
        void touchEntry(CacheType type, std::uint64_t id);
        void removeEntry(CacheType type, std::uint64_t id);
        void removeQueuedId(std::uint64_t id, CacheType type);
        void workerLoop();
        bool put(CacheType type, const NfPhotoId& id, const NfImage& image);
        bool evictToFit(std::uint64_t requiredBytes, CacheType protectedType,
                        std::uint64_t protectedId);
        std::filesystem::path entryPath(CacheType type, const NfPhotoId& id) const;
        std::filesystem::path entryPath(CacheType type, std::uint64_t id) const;

        NfDiskCacheSettings m_settings;
        std::mutex m_queueMutex;
        std::unordered_set<std::uint64_t> m_queuedThumbnailIds;
        std::unordered_set<std::uint64_t> m_queuedPreviewIds;
        std::mutex m_mutex;
        std::condition_variable m_queueCondition;
        std::deque<WriteRequest> m_writeQueue;
        std::list<CacheEntry> m_lruOrder;
        std::map<std::pair<CacheType, std::uint64_t>,
                 std::list<CacheEntry>::iterator> m_lruOrderById;
        bool m_stopping{false};
        std::thread m_worker;
        std::uint64_t m_currentCacheSize{0};
};

} // namespace NfCore

#endif // NF_DISK_CACHE_H
