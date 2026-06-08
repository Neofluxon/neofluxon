/**
 * File name: NfLibraryDatabase.h
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

#ifndef NF_LIBRARY_DATABASE_H
#define NF_LIBRARY_DATABASE_H

#include <sqlite3.h>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <string_view>

namespace NfCore {

/**
 * General Library structure
 *
 * [>] DateTime - Datetime tree strcuture: Captured via EXIF:CreateDate.
 *
 * [>] Canonical - Spatial/Physical Pillar: The "Source of Truth" location.
 *
 * [>] Collections - Intent/Curation Pillar. User organization.
 *
 * [>] Equipment - Technical Pillar: Hardware metadata (Camera & Lens).
 */

class NfRepresentationRecord;
class NfSourceRecord;
class NfSourceData;
class NfLibraryEntry;

class NfLibraryDatabase {
public:
        class Transaction {
        public:
                enum class Mode {
                        Transaction,
                        LockOnly
                };

                Transaction(NfLibraryDatabase* db, Mode mode = Mode::Transaction);
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

        explicit NfLibraryDatabase(const std::filesystem::path& dbPath);
        ~NfLibraryDatabase();
        NfLibraryDatabase(const NfLibraryDatabase&) = delete;
        NfLibraryDatabase& operator=(const NfLibraryDatabase&) = delete;
        NfLibraryDatabase(NfLibraryDatabase&&) = delete;
        NfLibraryDatabase& operator=(NfLibraryDatabase&&) = delete;

        const std::filesystem::path& path() const;

        bool open();
        void close();
        bool initializeSchema();

        int64_t addLibrary(const std::string_view name);
        bool libraryExists(int64_t id) const;
        std::unique_ptr<NfLibraryEntry> library(uint64_t id) const;
        std::unique_ptr<NfLibraryEntry> library(std::string_view name) const;
        std::vector<uint64_t> libraryIds() const;
        std::vector<NfLibraryEntry> libraries() const;

        int64_t addFolder(const std::filesystem::path& path, int64_t libraryId);
        int64_t addImage(int64_t folderId,
                         const std::string& fileName,
                         int64_t timestamp,
                         int64_t cameraId,
                         int64_t lensId);
        int64_t addCamera(std::string_view maker,
                          std::string_view model);
        int64_t addLens(std::string_view lens);
        std::unique_ptr<NfRepresentationRecord> getRepresentationRecord(int id);
        int64_t getOrCreateEquipment(const std::string& type,
                                     const std::string& make,
                                     const std::string& model);
        int64_t createCollection(const std::string& name, int64_t parentId = 0);
        bool addImageToCollection(int64_t imageId, int64_t collectionId);

protected:
        void loadDateTimeSource(std::unique_ptr<NfSourceRecord>& source);
        void populateSourceData(const std::unique_ptr<NfRepresentationRecord> &record);
        void loadCanonicalSource(std::unique_ptr<NfSourceData>& source);
        void loadEquipmentSource(std::unique_ptr<NfSourceData>& source);
        void loadCollectionsSource(std::unique_ptr<NfSourceData>& source);


private:
        mutable std::mutex m_mutex;
        struct sqlite3* m_db = nullptr;
        std::filesystem::path m_dbPath;
};

} // namespace NfCore

#endif // NF_LIBRARY_DATABASE_H
