/**
 * File name: NfLibraryRepresentation.cpp
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

#include "NfLibraryRepresentation.h"
#include "NfLibraryDatabase.h"
#include "NfSourceRecords.h"
#include "NfLibraryTreeNode.h"
#include "NfLibraryQuery.h"
#include "NfTimeUtils.h"
#include "NfFileUtils.h"
#include "NfLogger.h"
#include "NfStringUtils.h"

#include <unordered_map>
#include <chrono>
#include <format>

namespace NfCore {

NfLibraryRepresentation::NfLibraryRepresentation(NfLibraryDatabase *db,
                                                 int64_t libraryId,
                                                 RepresentationType t)
        : m_database{db}
        , m_libraryId{libraryId}
        , m_type{t}
{
}

NfLibraryRepresentation::~NfLibraryRepresentation()
{
}

NfLibraryRepresentation::RepresentationType
NfLibraryRepresentation::type() const
{
        return m_type;
}

void NfLibraryRepresentation::setName(std::string_view name)
{
        m_name = name;
}

const std::string& NfLibraryRepresentation::name()
{
        if (!m_name.empty())
                return m_name;

        auto record = m_database->getRepresentationRecord(m_libraryId, m_type);
        if (record)
                m_name = record->name;

        return m_name;
}

NfLibraryTreeNode* NfLibraryRepresentation::getTree()
{
        if (m_tree)
                return m_tree.get();

        auto record = m_database->getRepresentationRecord(m_libraryId, m_type);
        if (record) {
                m_name = record->name;
                populateTree(record.get());
        }

        return m_tree.get();
}

std::vector<NfPhoto> NfLibraryRepresentation::queryPhotos(const NfLibraryQuery &query) const
{
        NF_LOG_DEBUG("Representation; queryPhotos");
        switch (query.representationType) {
        case RepresentationType::Canonical:
                NF_LOG_DEBUG("Representation: Canonical");

                if (const auto *pathId = std::get_if<int64_t>(&query.queryValue))
                        return queryPhotosByPathId(*pathId);
                break;
        case RepresentationType::DateTime:
                NF_LOG_DEBUG("Representation: DateTime");

                if (const auto *dateRange = std::get_if<NfDateRange>(&query.queryValue))
                        return queryPhotosByDateRange(*dateRange);
                break;
        case RepresentationType::Equipment:
                NF_LOG_DEBUG("Representation: Equipment");
                if (const auto *equipment = std::get_if<NfEquipmentQuery>(&query.queryValue))
                        return queryPhotosByEquipment(*equipment);
                break;
        case RepresentationType::Collections:
                NF_LOG_DEBUG("Representation: Collections");
                break;
        case RepresentationType::None:
        default:
                break;
                NF_LOG_DEBUG("Representation: None");
        }

        return {};
}

std::vector<NfPhoto> NfLibraryRepresentation::queryPhotosByPathId(uint64_t id) const
{
        NF_LOG_DEBUG("called");
        const NfImageRecords imageRecords = m_database->getImagesByFolderId(id, m_libraryId);
        return imageRecordsToPhotos(imageRecords);
}

std::vector<NfPhoto>
NfLibraryRepresentation::queryPhotosByDateRange(const NfDateRange &dateRange) const
{
        NF_LOG_DEBUG("range: " << dateRange.start_ticks << ", " << dateRange.end_ticks);

        const NfImageRecords imageRecords = m_database->getImagesByDate(dateRange, m_libraryId);
        return imageRecordsToPhotos(imageRecords);
}

std::vector<NfPhoto>
NfLibraryRepresentation::queryPhotosByEquipment(const NfEquipmentQuery& equipment) const
{
        const NfImageRecords imageRecords = m_database->getImagesByEquipment(equipment,
                                                                   m_libraryId);
        return imageRecordsToPhotos(imageRecords);
}

void NfLibraryRepresentation::populateTree(const NfRepresentationRecord *rep)
{
        NF_LOG_DEBUG("called");

        switch (static_cast<RepresentationType>(rep->type)) {
        case RepresentationType::Canonical:
                populateCanonicalTree(rep);
                break;
        case RepresentationType::DateTime:
                populateDateTimeTree(rep);
                break;
        case RepresentationType::Equipment:
                populateEquipmentTree(rep);
                break;
        case RepresentationType::Collections:
                break;
        case RepresentationType::None:
        default:
                break;
        }
}

void NfLibraryRepresentation::populateDateTimeTree(const NfRepresentationRecord* rep)
{
        using NodeType = NfLibraryTreeNode::NodeType;
        namespace ch = std::chrono;

        m_tree = std::make_unique<NfLibraryTreeNode>("Root", NodeType::Root);

        auto* source = dynamic_cast<const NfDatetimeSourceRecord*>(rep->sourceData.get());
        if (!source)
                return;

        for (const auto& entry : source->entries) {
                ch::sys_time<ch::nanoseconds> tp{
                        ch::nanoseconds{entry.timestamp}
                };

                ch::zoned_time local{ch::current_zone(), tp};
                auto localDay = ch::floor<ch::days>(local.get_local_time());

                ch::year_month_day ymd{localDay};
                ch::year year = ymd.year();
                ch::month month = ymd.month();
                ch::day day = ymd.day();

                auto* parent = m_tree.get();

                // Year
                {
                        auto name = std::to_string(int(year));
                        parent = findOrCreateChild(parent,
                                                   name,
                                                   NodeType::DateYear);
                        parent->setValue(NfTimeUtils::getYearRange(year));
                }

                // Month
                {
                        auto name = std::format("{:02}", unsigned(month));
                        parent = findOrCreateChild(parent,
                                                   name,
                                                   NodeType::DateMonth);

                        parent->setValue(NfTimeUtils::getMonthRange(year / month));
                }

                // Day
                {
                        auto name = std::format("{:02}", unsigned(day));
                        parent = findOrCreateChild(parent,
                                                   name,
                                                   NodeType::DateDay);

                        parent->setValue(NfTimeUtils::getDayRange(year / month / day));
                }
        }
}

void NfLibraryRepresentation::populateEquipmentTree(const NfRepresentationRecord* rep)
{
        using NodeType = NfLibraryTreeNode::NodeType;

        m_tree = std::make_unique<NfLibraryTreeNode>("Root", NodeType::Root);

        auto* source = dynamic_cast<const NfEquipmentSourceRecord*>(rep->sourceData.get());
        if (!source)
                return;

        // Load cameras tree
        if (!source->cameras.empty()) {
                auto camerasRootNode = findOrCreateChild(m_tree.get(),
                                                         "Cameras",
                                                         NodeType::CameraCategory);
                if (camerasRootNode)
                        loadCamerasTree(camerasRootNode, source);
        }

        // Load lensess tree
        if (!source->lenses.empty()) {
                auto lensesRootNode = findOrCreateChild(m_tree.get(),
                                                        "Lenses",
                                                        NodeType::LensCategory);
                if (lensesRootNode)
                        loadLensesTree(lensesRootNode, source);
        }
}

void NfLibraryRepresentation::loadCamerasTree(NfLibraryTreeNode *parent,
                                              const NfEquipmentSourceRecord *source)
{
        for (auto const &camera: source->cameras) {
                auto name = camera.maker + " " + camera.model;
                auto childNode = findOrCreateChild(parent,
                                                   name,
                                                   NfLibraryTreeNode::NodeType::Camera);
                childNode->setValue(camera.id);
        }
}

void NfLibraryRepresentation::loadLensesTree(NfLibraryTreeNode *parent,
                                             const NfEquipmentSourceRecord *source)
{
        for (const auto& lens : source->lenses) {
                auto childNode = findOrCreateChild(parent,
                                                   lens.name,
                                                   NfLibraryTreeNode::NodeType::Lens);
                childNode->setValue(lens.id);
        }
}

NfLibraryTreeNode*
NfLibraryRepresentation::findOrCreateChild(NfLibraryTreeNode* parent,
                                           const std::string& name,
                                           NfLibraryTreeNode::NodeType nodeType)
{
        // search existing children
        for (const auto& child : parent->children()) {
                if (NfStringUtils::compareNoCase(child->name(), name))
                        return child.get();
        }

        // Not found, create
        auto* node = parent->addChild();
        node->setName(name);
        node->setType(nodeType);

        return node;
}

void NfLibraryRepresentation::populateCanonicalTree(const NfRepresentationRecord* rep)
{
        NF_LOG_DEBUG("called");

        m_tree = std::make_unique<NfLibraryTreeNode>(
                                                     "Root",
                                                     NfLibraryTreeNode::NodeType::Root);

        const auto* source =
                dynamic_cast<const NfCanonicalSourceRecord*>(rep->sourceData.get());

        if (!source) {
                NF_LOG_DEBUG("source record is null");
                return;
        }

        if (source->folders.empty())
                return;

        // Sort folders so that parents are processed before children.
        std::vector<NfFolderEntry> folders = source->folders;

        std::sort(folders.begin(),
                  folders.end(),
                  [](const auto& a, const auto& b)
                  {
                          return a.path < b.path;
                  });

        // Maps an imported folder path to its corresponding tree node.
        std::unordered_map<std::filesystem::path, NfLibraryTreeNode*> nodeMap;
        nodeMap.reserve(folders.size());

        for (const auto& folder : folders) {
                auto* parent = m_tree.get();

                // Find the nearest imported ancestor.
                auto parentPath = folder.path.parent_path();
                while (!parentPath.empty()) {
                        auto it = nodeMap.find(parentPath);
                        if (it != nodeMap.end()) {
                                parent = it->second;
                                break;
                        }

                        auto next = folder.path.parent_path();

                        // Check if this is a root path
                        if (next == parentPath)
                                break;

                        parentPath = std::move(next);
                }

                auto* node = parent->addChild();
                node->setName(NfFileUtils::pathToUtf8(folder.path.filename()));
                node->setType(NfLibraryTreeNode::NodeType::Folder);
                node->setValue(folder.id);

                nodeMap.emplace(folder.path, node);
        }
}

std::vector<NfPhoto>
NfLibraryRepresentation::imageRecordsToPhotos(const NfImageRecords& records)
{
        std::vector<NfPhoto> photos;
        for (const auto& record: records) {
                NfPhoto photo(record.path);
                if (!photo.id().isValid()) {
                        NF_LOG_DEBUG("photo invalid: " << photo.path());
                        continue;
                }

                photo.setName(record.name);
                photo.setSizeOnDisk(record.sizeOnDisk);
                photo.setFormat(record.format);
                photo.setDateTaken(record.dateTaken);
                photos.emplace_back(std::move(photo));
        }

        return photos;
}

} // namespace NfCore
