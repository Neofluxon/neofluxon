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
#include "NfLogger.h"

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
        switch (query.representationType) {
        case RepresentationType::Canonical:
                if (const auto *pathId = std::get_if<int64_t>(&query.queryValue))
                        return queryPhotosByPathId(*pathId);
                break;
        case RepresentationType::DateTime:
                if (const auto *dateTime = std::get_if<int64_t>(&query.queryValue))
                        return queryPhotosByPathId(*pathId);
                break;
        case RepresentationType::Equipment:
                break;
        case RepresentationType::Collections:
                break;
        case RepresentationType::None:
        default:
                break;
        }

        return {};
}

std::vector<NfPhoto> NfLibraryRepresentation::queryPhotosByPathId(uint64_t id) const
{
        std::vector<NfPhoto> photos;
        auto imagePaths = m_database->getImagePathsByFolderId(id);
        for (const auto &path : imagePaths)
                photos.emplace_back(path);

        return photos;
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
        using NodeType NfLibraryTreeNode::NodeType::Root;
        using namespace ch = std::chrono;

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
                        parent->setNodeValue(NfTimeUtils::getYearRange(year));
                }

                // Month
                {
                        auto name = std::format("{:02}", unsigned(month));
                        parent = findOrCreateChild(parent,
                                                   name,
                                                   NodeType::DateMonth);

                        parent->setNodeValue(NfTimeUtils::getMonthRange(year, month));
                }

                // Day
                {
                        auto name = std::format("{:02}", unsigned(day));
                        parent = findOrCreateChild(parent,
                                                   name,
                                                   NodeType::DateDay);

                        parent->setNodeValue(NfTimeUtils::getDayRange(year, month, day));
                }
        }
}

NfLibraryTreeNode*
NfLibraryRepresentation::findOrCreateChild(NfLibraryTreeNode* parent,
                                           const std::string& name,
                                           NfLibraryTreeNode::NodeType nodeType)
{
        // search existing children
        for (const auto& child : parent->children()) {
                if (child->name() == name)
                        return child.get();
        }

        // Not found, create
        auto* node = parent->addChild();
        node->setName(name);
        node->setType(nodeType);

        return node;
}

#include <chrono>

NfLibraryTreeNode::DateRange NfLibraryRepresentation::getYearRange(const std::tm& tm)
{
        using namespace std::chrono;

        int yearValue = tm.tm_year + 1900;

        sys_time<nanoseconds> start =
                sys_days{year{yearValue} / January / 1};

        sys_time<nanoseconds> end =
                sys_days{year{yearValue + 1} / January / 1};

        return {
                start.time_since_epoch().count(),
                end.time_since_epoch().count()
        };
}


NfLibraryTreeNode::DateRange NfLibraryRepresentation::getMonthRange(const std::tm& tm)
{
        using namespace std::chrono;

        year_month monthValue{
                year{tm.tm_year + 1900},
                month{static_cast<unsigned>(tm.tm_mon + 1)}
        };

        sys_time<nanoseconds> start =
                sys_days{monthValue / day{1}};

        sys_time<nanoseconds> end =
                sys_days{(monthValue + months{1}) / day{1}};

        return {
                start.time_since_epoch().count(),
                end.time_since_epoch().count()
        };
}


NfLibraryTreeNode::DateRange NfLibraryRepresentation::getDayRange(const std::tm& tm)
{
        using namespace std::chrono;

        sys_days dayValue =
                year{tm.tm_year + 1900} /
                month{static_cast<unsigned>(tm.tm_mon + 1)} /
                day{static_cast<unsigned>(tm.tm_mday)};

        sys_time<nanoseconds> start = dayValue;
        sys_time<nanoseconds> end = dayValue + days{1};

        return {
                start.time_since_epoch().count(),
                end.time_since_epoch().count()
        };
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
                node->setName(folder.path.filename().string());
                node->setType(NfLibraryTreeNode::NodeType::Folder);
                node->setValue(folder.id);

                nodeMap.emplace(folder.path, node);
        }
}

} // namespace NfCore
