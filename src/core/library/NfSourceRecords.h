/**
 * File name: NfSourceRecords.h
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

#ifndef NF_SOURCE_RECORDS_H
#define NF_SOURCE_RECORDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace NfCore {

// Base class for all source data containers
struct NfSourceRecord {
        virtual ~NfSourceRecord() = default;
};

struct NfLibraryEntry {
        uint64_t id;
        std::string name;
};

struct NfLibrariesSourceRecord : public NfSourceRecord {
        std::vector<NfLibraryEntry> libraries;
};

struct NfRepresentationRecord {
        int64_t libraryId;
        std::string name;
        int type;
        std::unique_ptr<NfSourceRecord> sourceData;
};

// Represents the Year/Month/Day tree data
struct NfDatetimeEntry {
        int64_t timestamp;
};

struct NfDatetimeSourceRecord : public NfSourceRecord {
        std::vector<NfDatetimeEntry> entries;
};

// Represents the physical folder structure
struct NfFolderEntry {
        int id;
        std::filesystem::path path;
};

struct NfCanonicalSourceRecord : public NfSourceRecord {
        std::vector<NfFolderEntry> folders;
};

// Represents Cameras and Lenses
struct NfCameraEntry {
        int id;
        std::string maker;
        std::string model;
};

struct NfLensEntry {
        int id;
        std::string name;
};

struct NfEquipmentSourceRecord : public NfSourceRecord {
        std::vector<NfCameraEntry> cameras;
        std::vector<NfLensEntry> lenses;
};

// Represents Virtual Albums
struct NfCollectionEntry {
        int id;
        std::string name;
};

struct NfCollectionsSourceRecord : public NfSourceRecord {
        std::vector<NfCollectionEntry> collections;
};

} // namespace NfCore

#endif // NF_SOURCE_RECORDS_H
