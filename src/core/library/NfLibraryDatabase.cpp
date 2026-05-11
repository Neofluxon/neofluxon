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
#include <iostream>

NfLibraryDatabase::NfLibraryDatabase(const std::filesystem::path& dbPath)
        : m_path(dbPath)
{
}

NfLibraryDatabase::~NfLibraryDatabase()
{
        close();
}

bool NfLibraryDatabase::open()
{
        return sqlite3_open(m_path.c_str(), &m_db) == SQLITE_OK;
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

bool NfLibraryDatabase::initializeSchema()
{
        const char* sql = R"(
        PRAGMA journal_mode = WAL;
        PRAGMA synchronous = NORMAL;
        PRAGMA foreign_keys = ON;

        CREATE TABLE IF NOT EXISTS folders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            path TEXT UNIQUE NOT NULL
        );

        CREATE TABLE IF NOT EXISTS equipment (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT CHECK(type IN ('CAMERA', 'LENS')),
            make TEXT,
            model TEXT UNIQUE
        );

        CREATE TABLE IF NOT EXISTS images (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            folder_id INTEGER,
            file_name TEXT,
            relative_path TEXT,
            FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS metadata (
            image_id INTEGER PRIMARY KEY,
            timestamp INTEGER,
            rating INTEGER DEFAULT 0,
            camera_id INTEGER,
            lens_id INTEGER,
            FOREIGN KEY (image_id) REFERENCES images(id) ON DELETE CASCADE,
            FOREIGN KEY (camera_id) REFERENCES equipment(id),
            FOREIGN KEY (lens_id) REFERENCES equipment(id)
        );

        CREATE INDEX IF NOT EXISTS idx_ts ON metadata(timestamp);)";

        return sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

int64_t NfLibraryDatabase::getOrCreateEquipment(const std::string& type,
                                                const std::string& make,
                                                const std::string& model)
{
        std::string sql = "INSERT OR IGNORE INTO equipment (type, make, model) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, make.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, model.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Now get the ID (either the new one or the existing one)
        sql = "SELECT id FROM equipment WHERE model = ?;";
        sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, model.c_str(), -1, SQLITE_STATIC);
        int64_t id = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW)
                id = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);

        return id;
}

bool NfLibraryDatabase::addImage(int64_t folderId,
                                 const NfImageEntry& entry,
                                 int64_t cameraId,
                                 int64_t lensId)
{
        sqlite3_stmt* stmt;

        // 1. Insert into images table
        const char* imgSql = "INSERT INTO images (folder_id, file_name, relative_path) VALUES (?, ?, ?);";
        sqlite3_prepare_v2(m_db, imgSql, -1, &stmt, nullptr);
        sqlite3_bind_int64(stmt, 1, folderId);
        sqlite3_bind_text(stmt, 2, entry.fileName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, entry.relativePath.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                return false;
        }
        int64_t lastId = sqlite3_last_insert_rowid(m_db);
        sqlite3_finalize(stmt);

        // 2. Insert into metadata table
        const char* metaSql = "INSERT INTO metadata (image_id, timestamp, rating, camera_id, lens_id) VALUES (?, ?, ?, ?, ?);";
        sqlite3_prepare_v2(m_db, metaSql, -1, &stmt, nullptr);
        sqlite3_bind_int64(stmt, 1, lastId);
        sqlite3_bind_int64(stmt, 2, entry.timestamp);
        sqlite3_bind_int(stmt, 3, entry.rating);
        sqlite3_bind_int64(stmt, 4, cameraId);
        sqlite3_bind_int64(stmt, 5, lensId);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return true;
}
