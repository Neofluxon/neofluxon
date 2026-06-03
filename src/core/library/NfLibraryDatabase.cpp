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

NfLibraryDatabase::Transaction::Transaction(NfLibraryDatabase* db)
        : m_db{db->m_db}
{
        sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
}

NfLibraryDatabase::Transaction::~Transaction()
{
        if (!m_committed)
                sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
}

void NfLibraryDatabase::Transaction::commit()
{
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

std::vector<uint64_t> NfLibraryDatabase::libraries() const
{
        return {0, 1, 2, 3};
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

        CREATE TABLE IF NOT EXISTS cameras (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            maker   TEXT,
            model   TEXT
        );

        CREATE TABLE IF NOT EXISTS lenses (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT
        );

        CREATE TABLE IF NOT EXISTS folders (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            path    TEXT UNIQUE NOT NULL
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

        -- Virtual grouping of images (e.g., "Best of Orchard 2026")
        CREATE TABLE IF NOT EXISTS collections (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            name        TEXT NOT NULL UNIQUE,
            description TEXT,
            created_at  INTEGER
        );

        -- Link table for many-to-many relationship
        CREATE TABLE IF NOT EXISTS collection_images (
            collection_id INTEGER,
            image_id      INTEGER,
            position      INTEGER, -- Optional: to allow custom sorting within a collection
            PRIMARY KEY (collection_id, image_id),
            FOREIGN KEY (collection_id) REFERENCES collections(id) ON DELETE CASCADE,
            FOREIGN KEY (image_id)      REFERENCES images(id)      ON DELETE CASCADE
        );

        CREATE INDEX IF NOT EXISTS idx_images_folder ON images(folder_id);
        CREATE INDEX IF NOT EXISTS idx_images_datetime ON images(datetime_taken);
        CREATE INDEX IF NOT EXISTS idx_coll_img_id ON collection_images(image_id);)";

        return sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

int64_t NfLibraryDatabase::addFolder(const std::string& absolutePath)
{
        const char* sql = "INSERT OR IGNORE INTO folders (path) VALUES (?);";
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
                return -1;

        sqlite3_bind_text(stmt, 1, absolutePath.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);
        int64_t resultId = -1;

        if (rc == SQLITE_DONE) {
                // Check if a row was actually inserted
                if (sqlite3_changes(m_db) > 0) {
                        resultId = sqlite3_last_insert_rowid(m_db);
                } else {
                        // Folder already existed. Let's fetch its existing ID.
                        sqlite3_finalize(stmt);

                        const char* selectSql = "SELECT id FROM folders WHERE path = ?;";
                        if (sqlite3_prepare_v2(m_db, selectSql, -1, &stmt, nullptr) == SQLITE_OK) {
                                sqlite3_bind_text(stmt, 1, absolutePath.c_str(), -1, SQLITE_TRANSIENT);
                                if (sqlite3_step(stmt) == SQLITE_ROW) {
                                        resultId = sqlite3_column_int64(stmt, 0);
                                }
                        }
                }
        }

        sqlite3_finalize(stmt);

        return resultId;
}

int64_t NfLibraryDatabase::addImage(int64_t folderId,
                                    const std::string& fileName,
                                    int64_t timestamp,
                                    int64_t cameraId,
                                    int64_t lensId)
{
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
