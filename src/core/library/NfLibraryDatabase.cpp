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

#include <iostream>

namespace NfCore {

NfLibraryDatabase::NfLibraryDatabase(const std::filesystem::path& dbPath)
        : m_dbPath{dbPath}
{
}

NfLibraryDatabase::~NfLibraryDatabase()
{
        close();
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

void NfLibraryDatabase::beginTransaction()
{
        sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
}

void NfLibraryDatabase::endTransaction()
{
        sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);
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
        /*const char* sql = R"(
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
        */
        return false;//sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

} // namespace NfCore
