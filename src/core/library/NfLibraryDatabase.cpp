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

#include "NfLibraryDatabase.h"
#include "NfSourceRecords.h"
#include "NfLogger.h"

#include <iostream>

namespace NfCore {

        NfLibraryDatabase::Transaction::Transaction(NfLibraryDatabase* db,
                                                    Mode mode)
        : m_db{db->m_db}
        , m_mode{mode}
        , m_lock{db->m_mutex}
{
        if (m_mode == Mode::Transaction)
                sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
}

NfLibraryDatabase::Transaction::~Transaction()
{
        if (m_mode == Mode::Transaction && !m_committed)
                sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
}

void NfLibraryDatabase::Transaction::commit()
{
        if (m_mode != Mode::Transaction)
                return;

        if (!m_committed) {
                sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);
                m_committed = true;
        }
}

NfLibraryDatabase::NfLibraryDatabase(const std::filesystem::path& dbPath)
        : m_dbPath{dbPath}
{
        if(!open()) {
                NF_LOG_ERROR("can't open DB for DB: " << m_dbPath );
        }
}

NfLibraryDatabase::~NfLibraryDatabase()
{
        close();
}

const std::filesystem::path& NfLibraryDatabase::path() const
{
        return m_dbPath;
}

bool NfLibraryDatabase::open()
{
        return sqlite3_open(m_dbPath.c_str(), &m_db) == SQLITE_OK;
}

void NfLibraryDatabase::close()
{
        if (m_db) {
                sqlite3_close(m_db);
                m_db = nullptr;
        }
}

std::unique_ptr<NfRepresentationRecord> NfLibraryDatabase::getRepresentationRecord(int id)
{
        /*sqlite3_stmt* stmt;
        const char* sql = "SELECT name, type FROM representations WHERE id = ?";

        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return nullptr;

        sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);

        std::unique_ptr<NfRepresentationRecord> record;
        if (rc == SQLITE_ROW) {
                record = std::make_unique<NfRepresentationRecord>();
                record->id = id;

                const unsigned char* rawName = sqlite3_column_text(stmt, 0);
                int rawType = sqlite3_column_int(stmt, 1);

                record->name = reinterpret_cast<const char*>(rawName);
                record->type = rawType;
        }

        sqlite3_finalize(stmt);

        populateSourceData(record);*/

        return nullptr; //record;
}

/*std::vector<std::unique_ptr<NfRepresentationRecord>>
NfLibraryDatabase::getRepresentations(int libraryId) const
{
}*/

void NfLibraryDatabase::populateSourceData(const std::unique_ptr<NfRepresentationRecord> &record)
{
        /*switch (record->type) {
        case NfRepresentationType::DateTime:
                record->sourceData = std::make_unique<NfDatetimeSourceRecord>();
                loadDateTimeSource(record->sourceData);
                break;
        case NfRepresentationType::Canonical:
                record->sourceData = std::make_unique<NfCanonicalSourceRecord>();
                loadCanonicalSource(record->sourceData);
                break;
        case NfRepresentationType::Equipment:
                record->sourceData = std::make_unique<NfEquipmentSourceRecord>();
                loadEquipmentSource(record->sourceData);
                break;
        case NfRepresentationType::Collections:
                record->sourceData = std::make_unique<NfCollectionsSourceRecord>();
                loadCollectionsSource(record->sourceData);
                break;
                }*/
}

void NfLibraryDatabase::loadDateTimeSource(std::unique_ptr<NfSourceRecord>& source)
{
        /*auto* record = static_cast<NfDatetimeSourceRecord*>(source.get());
        const char* sql = "SELECT DISTINCT datetime_taken "
                "FROM images "
                "WHERE datetime_taken IS NOT NULL;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW)
                        record->entries.push_back({ sqlite3_column_int64(stmt, 0) });
        }
        sqlite3_finalize(stmt);*/
}

void NfLibraryDatabase::loadCanonicalSource(std::unique_ptr<NfSourceData>& source)
{
        /*auto* record = static_cast<NfCanonicalSourceRecord*>(source.get());

    // We join images and folders to find only folders that actually contain images
    const char* sql =
        "SELECT DISTINCT f.id, f.path "
        "FROM folders f "
        "INNER JOIN images i ON f.id = i.folder_id "
        "ORDER BY f.path ASC;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            // Storing the ID and Path pair
            record->folders.push_back({ id, path ? path : "" });
        }
    } else {
        // Log error: sqlite3_errmsg(m_db)
    }

    sqlite3_finalize(stmt);*/
}

void NfLibraryDatabase::loadEquipmentSource(std::unique_ptr<NfSourceData>& source)
{
        /*auto* record = static_cast<NfEquipmentSourceRecord*>(source.get());
        sqlite3_stmt* stmt;

        // 1. Load only Cameras that have associated images
        const char* sqlCameras =
                "SELECT DISTINCT c.id, c.maker, c.model "
                "FROM cameras c "
                "INNER JOIN images i ON c.id = i.camera_id "
                "ORDER BY c.maker ASC, c.model ASC;";

        if (sqlite3_prepare_v2(m_db, sqlCameras, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                        int id = sqlite3_column_int(stmt, 0);
                        const char* maker = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                        const char* model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

                        record->cameras.push_back({
                                        id,
                                        maker ? maker : "",
                                        model ? model : ""
                                });
                }
        }
        sqlite3_finalize(stmt);

        // 2. Load only Lenses that have associated images
        const char* sqlLenses =
                "SELECT DISTINCT l.id, l.name "
                "FROM lenses l "
                "INNER JOIN images i ON l.id = i.lens_id "
                "ORDER BY l.name ASC;";

        if (sqlite3_prepare_v2(m_db, sqlLenses, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                        int id = sqlite3_column_int(stmt, 0);
                        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

                        record->lenses.push_back({
                                        id,
                                        name ? name : ""
                                });
                }
        }
        sqlite3_finalize(stmt);*/
}

void NfLibraryDatabase::loadCollectionsSource(std::unique_ptr<NfSourceData>& source)
{
        /*auto* record = static_cast<NfCollectionsSourceRecord*>(source.get());

        // Join collections with image_collections (or images) to find non-empty sets
        // Assuming a many-to-many relationship table 'image_collections'
        const char* sql =
                "SELECT DISTINCT c.id, c.name "
                "FROM collections c "
                "INNER JOIN image_collections ic ON c.id = ic.collection_id "
                "ORDER BY c.name ASC;";

        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                        int id = sqlite3_column_int(stmt, 0);
                        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

                        record->collections.push_back({ id, name ? name : "" });
                }
        }

        sqlite3_finalize(stmt);*/
}

bool NfLibraryDatabase::initializeSchema()
{
        const char* sql = R"(
        PRAGMA journal_mode = WAL;
        PRAGMA synchronous = NORMAL;
        PRAGMA foreign_keys = ON;

        CREATE TABLE IF NOT EXISTS libraries (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS folders (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            library_id  INTEGER NOT NULL,
            path        TEXT NOT NULL,

            FOREIGN KEY (library_id) REFERENCES libraries(id) ON DELETE CASCADE,
            UNIQUE (library_id, path)
        );

        CREATE TABLE IF NOT EXISTS cameras (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            maker   TEXT NOT NULL,
            model   TEXT NOT NULL,

            UNIQUE (maker, model)
        );

        CREATE TABLE IF NOT EXISTS lenses (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS images (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            folder_id       INTEGER NOT NULL,
            file_name       TEXT NOT NULL,
            lens_id         INTEGER,
            camera_id       INTEGER,
            datetime_taken  INTEGER,

            FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE,
            FOREIGN KEY (camera_id) REFERENCES cameras(id) ON DELETE SET NULL,
            FOREIGN KEY (lens_id)   REFERENCES lenses(id)  ON DELETE SET NULL
        );

        CREATE TABLE IF NOT EXISTS collections (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            library_id  INTEGER NOT NULL,
            name        TEXT NOT NULL,
            description TEXT,
            created_at  INTEGER,

            FOREIGN KEY (library_id) REFERENCES libraries(id) ON DELETE CASCADE,
            UNIQUE (library_id, name)
        );

        CREATE TABLE IF NOT EXISTS collection_images (
            collection_id INTEGER NOT NULL,
            image_id      INTEGER NOT NULL,
            position      INTEGER,

            PRIMARY KEY (collection_id, image_id),

            FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE,
            FOREIGN KEY (image_id)      REFERENCES images(id)      ON DELETE CASCADE
        );

       CREATE INDEX IF NOT EXISTS idx_images_folder
           ON images(folder_id);

       CREATE INDEX IF NOT EXISTS idx_images_datetime
          ON images(datetime_taken);

       CREATE INDEX IF NOT EXISTS idx_coll_img_id
          ON collection_images(image_id);)";

       return sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

int64_t NfLibraryDatabase::addLibrary(const std::string_view name)
{
        const char* sql = R"(
        INSERT INTO libraries (name)
        VALUES (?)
        ON CONFLICT(name) DO UPDATE SET name = excluded.name;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_text(stmt, 1, name.data(),
                          static_cast<int>(name.size()),
                          SQLITE_TRANSIENT);

        int64_t libraryId = -1;
        if (sqlite3_step(stmt) == SQLITE_DONE)
                libraryId = static_cast<int64_t>(sqlite3_last_insert_rowid(m_db));

        sqlite3_finalize(stmt);

        return libraryId;
}

bool NfLibraryDatabase::libraryExists(int64_t id) const
{
        const char* sql = R"(
        SELECT 1
        FROM libraries
        WHERE id = ?
        LIMIT 1;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return false;

        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(id));

        bool exists = false;

        if (sqlite3_step(stmt) == SQLITE_ROW)
                exists = true;

        sqlite3_finalize(stmt);

        return exists;
}

std::vector<uint64_t>
NfLibraryDatabase::libraryIds() const
{
        std::vector<uint64_t> ids;

        const char* sql = "SELECT id FROM libraries ORDER BY name;";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return ids;

        while (sqlite3_step(stmt) == SQLITE_ROW)
                ids.push_back(
                              static_cast<uint64_t>(
                                                    sqlite3_column_int64(stmt, 0)));

        sqlite3_finalize(stmt);

        return ids;
}

std::unique_ptr<NfLibraryEntry>
NfLibraryDatabase::library(uint64_t id) const
{
        const char* sql = R"(
        SELECT id, name
        FROM libraries
        WHERE id = ?;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return nullptr;

        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(id));

        std::unique_ptr<NfLibraryEntry> library;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
                library = std::make_unique<NfLibraryEntry>();
                library->id =
                        static_cast<uint64_t>(
                                sqlite3_column_int64(stmt, 0));

                if (const auto* text =
                    reinterpret_cast<const char*>(
                            sqlite3_column_text(stmt, 1))) {
                        library->name = text;
                }
        }

        sqlite3_finalize(stmt);

        return library;
}

std::unique_ptr<NfLibraryEntry>
NfLibraryDatabase::library(std::string_view name) const
{
        const char* sql = R"(
        SELECT id, name
        FROM libraries
        WHERE name = ?;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return nullptr;

        sqlite3_bind_text(stmt, 1,
                          name.data(),
                          static_cast<int>(name.size()),
                          SQLITE_TRANSIENT);

        std::unique_ptr<NfLibraryEntry> library;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
                library = std::make_unique<NfLibraryEntry>();
                library->id =
                        static_cast<uint64_t>(
                                sqlite3_column_int64(stmt, 0));

                if (const auto* text =
                    reinterpret_cast<const char*>(
                            sqlite3_column_text(stmt, 1))) {
                        library->name = text;
                }
        }

        sqlite3_finalize(stmt);

        return library;
}

std::vector<NfLibraryEntry>
NfLibraryDatabase::libraries() const
{
        std::vector<NfLibraryEntry> libraries;

        const char* sql = R"(
        SELECT id, name
        FROM libraries
        ORDER BY name;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return libraries;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
                NfLibraryEntry library;

                library.id =
                        static_cast<uint64_t>(
                                sqlite3_column_int64(stmt, 0));

                if (const auto* text =
                    reinterpret_cast<const char*>(
                            sqlite3_column_text(stmt, 1))) {
                        library.name = text;
                }

                libraries.push_back(std::move(library));
        }

        sqlite3_finalize(stmt);

        return libraries;
}

int64_t NfLibraryDatabase::addFolder(const std::filesystem::path& path,
                                     int64_t libraryId)
{
        const std::string pathStr = path.lexically_normal().generic_string();

        // 1. Insert (or ignore if already exists)
        const char* insertSql = R"(
        INSERT OR IGNORE INTO folders (library_id, path)
        VALUES (?, ?);
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_int64(stmt, 1, libraryId);
        sqlite3_bind_text(stmt, 2,
                           pathStr.c_str(),
                           -1,
                           SQLITE_TRANSIENT);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // 2. Always fetch the id (existing or newly inserted)
        const char* selectSql = R"(
        SELECT id
        FROM folders
        WHERE library_id = ? AND path = ?
        LIMIT 1;
        )";

        if (sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_int64(stmt, 1, libraryId);
        sqlite3_bind_text(stmt, 2,
                          pathStr.c_str(),
                          -1,
                          SQLITE_TRANSIENT);

        int64_t id = -1;

        if (sqlite3_step(stmt) == SQLITE_ROW)
                id = sqlite3_column_int64(stmt, 0);

        sqlite3_finalize(stmt);

        return id;
}

int64_t NfLibraryDatabase::addImage(int64_t folderId,
                                    const std::string& fileName,
                                    int64_t timestamp,
                                    int64_t cameraId,
                                    int64_t lensId)
{
        NF_LOG_DEBUG("folderId: " << folderId
                     << "timestamp: " << timestamp
                     << ", cameraId: " << cameraId
                     << ", cameraId: " << lensId
                     << fileName);
        NF_LOG_DEBUG("timestamp: " << timestamp);
        const char* sql = R"(
        INSERT INTO images (folder_id, file_name, datetime_taken, camera_id, lens_id)
        VALUES (?, ?, ?, ?, ?);)";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        // Bind parameters (1-indexed mapping to the '?' placeholders)
        sqlite3_bind_int64(stmt, 1, folderId);
        sqlite3_bind_text(stmt, 2, fileName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 3, timestamp);

        if (cameraId > 0)
                sqlite3_bind_int64(stmt, 4, cameraId);
        else
                sqlite3_bind_null(stmt, 4);

        if (lensId > 0)
                sqlite3_bind_int64(stmt, 5, lensId);
        else
                sqlite3_bind_null(stmt, 5);

        int64_t resultId = -1;
        if (sqlite3_step(stmt) == SQLITE_DONE)
                resultId = sqlite3_last_insert_rowid(m_db);

        sqlite3_finalize(stmt);

        return resultId;
}

int64_t NfLibraryDatabase::addCamera(std::string_view maker,
                                     std::string_view model)
{
        const char* selectSql = "SELECT id FROM cameras WHERE maker = ? AND model = ?;";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt,
                                  1, maker.data(), static_cast<int>(maker.size()),
                                  SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt,
                                  2, model.data(), static_cast<int>(model.size()),
                                  SQLITE_TRANSIENT);

                if (sqlite3_step(stmt) == SQLITE_ROW) {
                        int64_t existingId = sqlite3_column_int64(stmt, 0);
                        sqlite3_finalize(stmt);
                        return existingId;
                }
        }
        sqlite3_finalize(stmt);

        const char* insertSql = "INSERT INTO cameras (maker, model) VALUES (?, ?);";
        if (sqlite3_prepare_v2(m_db, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_text(stmt, 1, maker.data(),
                          static_cast<int>(maker.size()), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, model.data(),
                          static_cast<int>(model.size()), SQLITE_TRANSIENT);

        int64_t resultId = -1;
        if (sqlite3_step(stmt) == SQLITE_DONE)
                resultId = sqlite3_last_insert_rowid(m_db);

        sqlite3_finalize(stmt);

        return resultId;
}

int64_t NfLibraryDatabase::addLens(std::string_view lens)
{
        const char* selectSql = "SELECT id FROM lenses WHERE name = ?;";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(stmt,
                                  1, lens.data(), static_cast<int>(lens.size()),
                                  SQLITE_TRANSIENT);

                if (sqlite3_step(stmt) == SQLITE_ROW) {
                        int64_t existingId = sqlite3_column_int64(stmt, 0);
                        sqlite3_finalize(stmt);
                        return existingId;
                }
        }
        sqlite3_finalize(stmt);

        const char* insertSql = "INSERT INTO lenses (name) VALUES (?);";
        if (sqlite3_prepare_v2(m_db, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_text(stmt, 1, lens.data(),
                          static_cast<int>(lens.size()),
                          SQLITE_TRANSIENT);

        int64_t resultId = -1;
        if (sqlite3_step(stmt) == SQLITE_DONE)
                resultId = sqlite3_last_insert_rowid(m_db);

        sqlite3_finalize(stmt);

        return resultId;
}

} // namespace NfCore
