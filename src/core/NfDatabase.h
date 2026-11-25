/**
 * File name: NfDatabase.h
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

#ifndef NF_DATABASE_H
#define NF_DATABASE_H

#include "NfLibraryRepresentation.h"
#include "NfEquipmentQuery.h"
#include "NfSourceRecords.h"
#include "NfTimeUtils.h"

#include <cstdint>
#include <sqlite3.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <string_view>

namespace NfCore {

class NfRepresentationRecord;
class NfSourceRecord;
class NfLibraryEntry;
class NfLibraryQuery;

class NfDatabase {
public:
        class Transaction {
        public:
                enum class Mode {
                        Transaction,
                        LockOnly
                };

                Transaction(NfDatabase* db, Mode mode = Mode::Transaction);
                ~Transaction();
                Transaction(const Transaction&) = delete;
                Transaction& operator=(const Transaction&) = delete;
                void commit();

        private:
                struct sqlite3* m_db;
                Mode m_mode;
                bool m_committed = false;
                std::unique_lock<std::mutex> m_lock;
        };

        explicit NfDatabase(const std::filesystem::path& dbPath);
        ~NfDatabase();
        NfDatabase(const NfDatabase&) = delete;
        NfDatabase& operator=(const NfDatabase&) = delete;
        NfDatabase(NfDatabase&&) = delete;
        NfDatabase& operator=(NfDatabase&&) = delete;
        const std::filesystem::path& path() const;
        bool open(std::string& errorMessage);
        void close();
        bool initializeSchema(std::string& errorMessage);
        int64_t addLibrary(const std::string_view name);
        bool updateLibrary(int64_t id, std::string_view name);
        bool removeLibrary(int64_t id);
        bool libraryExists(int64_t id) const;
        std::unique_ptr<NfLibraryEntry> library(int64_t id) const;
        std::unique_ptr<NfLibraryEntry> library(std::string_view name) const;
        std::vector<int64_t> libraryIds() const;
        std::vector<NfLibraryEntry> libraries() const;
        int64_t addFolder(const std::filesystem::path& path, int64_t libraryId);
        int64_t addImage(uint64_t photo_id,
                         int64_t folderId,
                         const std::string& fileName,
                         NfPhoto::PhotoFormat format,
                         std::uintmax_t sizeOnDisk,
                         int64_t timestamp,
                         int64_t cameraId,
                         int64_t lensId,
                         int rating = 0);
        std::optional<NfDateRange> getDateTimeRange(int64_t libraryId) const;
        NfImageRecords getImages(const NfLibraryQuery &query) const;
        NfImageRecords getImagesByFolderId(int64_t folderId,
                                           int64_t libraryId) const;
        NfImageRecords getImagesByDate(const NfDateRange& dateRange,
                                       int64_t libraryId) const;
        NfImageRecords getImagesByEquipment(const NfEquipmentQuery& equipment,
                                            int64_t libraryId) const;
        int64_t getFolderIdByPath(const std::filesystem::path& folder,
                                  int64_t libraryId) const;
        int64_t addCamera(std::string_view maker,
                          std::string_view model);
        int64_t addLens(std::string_view lens);
        std::unique_ptr<NfRepresentationRecord> getRepresentationRecord(int64_t libraryId,
                                                                        NfLibraryRepresentation::RepresentationType repType);
        //int64_t getOrCreateEquipment(const std::string& type,
        //                             const std::string& make,
        //                             const std::string& model);
        //int64_t createCollection(const std::string& name, int64_t parentId = 0);
        //bool addImageToCollection(int64_t imageId, int64_t collectionId);
        bool setImagesFavorite(const std::vector<NfPhotoId>& photoIds,
                               bool favorite = true);
        bool setImagesRating(const std::vector<NfPhotoId>& photoIds,
                             int rating);

protected:
        void populateSourceData(NfRepresentationRecord *record);
        void loadDateTimeSource(NfSourceRecord *source, int64_t libraryId);
        void loadCanonicalSource(NfSourceRecord *source, int64_t libraryId);
        void loadEquipmentSource(NfSourceRecord *source, int64_t libraryId);
        void loadCollectionsSource(NfSourceRecord *source, int64_t libraryId);

private:
        mutable std::mutex m_mutex;
        struct sqlite3* m_db = nullptr;
        std::filesystem::path m_dbPath;
};

} // namespace NfCore

#endif // NF_LIBRARY_H
