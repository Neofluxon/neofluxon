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

// Objects representing the data layer
struct NfCamera { int64_t id; std::string make; std::string model; };
struct NfLens   { int64_t id; std::string make; std::string model; };

class NfRepresentationRecord;
class NfSourceRecord;
class NfSourceData;

class NfLibraryDatabase {
public:
        class Transaction {
        public:
                Transaction(NfLibraryDatabase* db);
                ~Transaction();
                void commit();

        private:
                struct sqlite3* m_db;
                bool m_committed = false;
        };

        explicit NfLibraryDatabase(const std::filesystem::path& dbPath);
        ~NfLibraryDatabase();
        const std::filesystem::path& path() const;

        bool open();
        void close();
        bool initializeSchema();

        int64_t addFolder(const std::string& absolutePath);
        int64_t addImage(int64_t folderId,
                         const std::string& fileName,
                         int64_t timestamp,
                         int64_t cameraId,
                         int64_t lensId);
        int64_t addCamera(std::string_view maker,
                          std::string_view model);
        int64_t addLens(std::string_view lens);

        std::vector<uint64_t> libraries() const;
        std::unique_ptr<NfRepresentationRecord> getRepresentationRecord(int id);


        // [HOW] Equipment (Returns ID of existing or new)
        int64_t getOrCreateEquipment(const std::string& type,
                                     const std::string& make,
                                     const std::string& model);

        // [WHY] Collections (The Tree Structure)
        int64_t createCollection(const std::string& name, int64_t parentId = 0);
        bool addImageToCollection(int64_t imageId, int64_t collectionId);

        // [THE CORE] Adding the Image and its Metadata
        // This connects Folder, Equipment, and the File together

protected:
        void loadDateTimeSource(std::unique_ptr<NfSourceRecord>& source);
        void populateSourceData(const std::unique_ptr<NfRepresentationRecord> &record);
        void loadCanonicalSource(std::unique_ptr<NfSourceData>& source);
        void loadEquipmentSource(std::unique_ptr<NfSourceData>& source);
        void loadCollectionsSource(std::unique_ptr<NfSourceData>& source);


private:
        std::unique_lock<std::mutex> m_lock;
        struct sqlite3* m_db = nullptr;
        std::filesystem::path m_dbPath;
};

} // namespace NfCore

#endif // NF_LIBRARY_DATABASE_H
