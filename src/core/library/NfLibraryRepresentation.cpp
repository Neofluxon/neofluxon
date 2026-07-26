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
        m_tree = std::make_unique<NfLibraryTreeNode>("Root",
                                                     NfLibraryTreeNode::NodeType::Root);

        auto* source = dynamic_cast<const NfDatetimeSourceRecord*>(rep->sourceData.get());
        if (!source)
                return;

        for (const auto& entry : source->entries) {
                std::time_t t = static_cast<std::time_t>(entry.timestamp / 1000000000LL);
                std::tm tm{};
#ifdef _WIN32
                localtime_s(&tm, &t);
#else
                localtime_r(&t, &tm);
#endif

                char yearBuf[8];
                char monthBuf[8];
                char dayBuf[8];

                std::strftime(yearBuf, sizeof(yearBuf), "%Y", &tm);
                std::strftime(monthBuf, sizeof(monthBuf), "%m", &tm);
                std::strftime(dayBuf, sizeof(dayBuf), "%d", &tm);

                NfLibraryTreeNode* parent = m_tree.get();

                // Year
                {
                        std::string year = yearBuf;
                        parent = findOrCreateChild(parent, year,
                                                   NfLibraryTreeNode::NodeType::DateYear);
                }

                // Month
                {
                        std::string month = monthBuf;
                        parent = findOrCreateChild(parent,
                                                   month,
                                                   NfLibraryTreeNode::NodeType::DateMonth);
                }

                // Day
                {
                        std::string day = dayBuf;
                        parent = findOrCreateChild(parent,
                                                   day,
                                                   NfLibraryTreeNode::NodeType::DateDay);
                }
        }
}

NfLibraryTreeNode* NfLibraryRepresentation::findOrCreateChild(NfLibraryTreeNode* parent,
                                                              const std::string& name,
                                                              NfLibraryTreeNode::NodeType nodeType){
        // search existing children
        for (const auto& child : parent->children()) {
                if (child->name() == name)
                        return child.get();
        }

        // not found -> create
        auto* node = parent->addChild();
        node->setName(name);
        node->setType(nodeType);

        return node;
}

void NfLibraryRepresentation::populateCanonicalTree(const NfRepresentationRecord* rep)
{
        NF_LOG_DEBUG("called");

        m_tree = std::make_unique<NfLibraryTreeNode>("Root",
                                                     NfLibraryTreeNode::NodeType::Root);

        const auto* source = dynamic_cast<const NfCanonicalSourceRecord*>(rep->sourceData.get());

        if (!source) {
                NF_LOG_DEBUG("source record is null");
                return;
        }

        if (source->folders.empty())
                return;

        // Find imported root folders
        std::vector<NfFolderEntry> roots;
        for (const auto& folder : source->folders) {
                bool isRoot = true;

                for (const auto& other : source->folders) {
                        if (folder.path == other.path)
                                continue;

                        std::error_code ec;
                        auto relative = std::filesystem::relative(
                                                                  folder.path,
                                                                  other.path,
                                                                  ec);

                        if (ec)
                                continue;

                        // folder is inside another folder
                        if (!relative.empty() &&
                            *relative.begin() != "..") {
                                isRoot = false;
                                break;
                        }
                }

                if (isRoot)
                        roots.push_back(folder);
        }

        NF_LOG_DEBUG("found roots: " << roots.size());

        // Build tree for each root
        for (const auto& root : roots) {
                const std::filesystem::path rootPath = root.path;

                for (const auto& folder : source->folders) {
                        std::error_code ec;
                        auto relative = std::filesystem::relative(
                                                                  folder.path,
                                                                  rootPath,
                                                                  ec);

                        if (ec)
                                continue;

                        // Skip folders that are not inside this root
                        if (!relative.empty() &&
                            *relative.begin() == "..")
                                continue;


                        auto* parent = m_tree.get();

                        // Add root folder name
                        std::filesystem::path treePath = rootPath.filename();

                        if (!relative.empty() && relative != ".") {
                                treePath /= relative;
                        }

                        for (const auto& part : treePath) {

                                const std::string name = part.string();

                                if (name.empty() || name == ".")
                                        continue;


                                NfLibraryTreeNode* child = nullptr;

                                // Find existing child
                                for (const auto& c : parent->children()) {
                                        if (c->name() == name) {
                                                child = c.get();
                                                break;
                                        }
                                }


                                // Create missing child
                                if (!child) {
                                        child = parent->addChild();
                                        child->setName(name);
                                        child->setType(NfLibraryTreeNode::NodeType::Folder);
                                }


                                parent = child;
                        }

                        // Only the real folder node gets the database id
                        parent->setValue(folder.id);
                }
        }
}

} // namespace NfCore
