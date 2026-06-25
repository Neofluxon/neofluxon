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
#include "NfLogger.h"

namespace NfCore {

NfLibraryRepresentation::NfLibraryRepresentation(NfLibraryDatabase *db,
                                                 int64_t libraryId,
                                                 RepresentationType t)
        : m_database{db}
        , m_libraryId{libraryId}
        , m_type{t}
{

        NF_LOG_DEBUG("called");
        auto record = m_database->getRepresentationRecord(m_libraryId, m_type);
        if (record) {
                m_name = record->name;
                NF_LOG_DEBUG("populate tree for : " << m_name);
                populateTree(record.get());
        }
}

NfLibraryRepresentation::~NfLibraryRepresentation()
{
}

void NfLibraryRepresentation::setName(std::string_view name)
{
        m_name = name;
}

const std::string& NfLibraryRepresentation::name() const
{
        return m_name;
}

NfLibraryTreeNode* NfLibraryRepresentation::getTree() const
{
        return m_tree.get();
}

void NfLibraryRepresentation::populateTree(const NfRepresentationRecord *rep)
{
        NF_LOG_DEBUG("called");
        switch (static_cast<RepresentationType>(rep->type)) {
        case RepresentationType::DateTime:
                populateDateTimeTree(rep);
                break;
        case RepresentationType::Canonical:
                populateCanonicalTree(rep);
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
        NF_LOG_DEBUG("called");
        m_tree = std::make_unique<NfLibraryTreeNode>("Root",
                                                     NfLibraryTreeNode::NodeType::Root);

        auto* source = dynamic_cast<const NfDatetimeSourceRecord*>(rep->sourceData.get());
        if (!source)
                return;

        NF_LOG_DEBUG("datetime entries: " << source->entries.size());
        for (const auto& entry : source->entries) {
                std::time_t t = static_cast<std::time_t>(entry.timestamp / 1000000000LL);
                NF_LOG_DEBUG("datetime raw ns: " << entry.timestamp);
                NF_LOG_DEBUG("datetime sec: " << t);

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

        auto* source = dynamic_cast<const NfCanonicalSourceRecord*>(rep->sourceData.get());

        if (!source) {
                NF_LOG_DEBUG("source record is null");
                return;
        }

        for (const auto& folder : source->folders) {
                NF_LOG_DEBUG("path: " << folder.path);

                auto* parent = m_tree.get();

                std::filesystem::path p = folder.path;
                for (const auto& part : p) {
                        std::string name = part.string();

                        if (name.empty())
                                continue;

                        NfLibraryTreeNode* child = nullptr;

                        for (const auto& c : parent->children()) {
                                if (c->name() == name) {
                                        child = c.get();
                                        break;
                                }
                        }

                        if (!child) {
                                child = parent->addChild();
                                child->setName(name);
                                child->setType(
                                               NfLibraryTreeNode::NodeType::Folder);
                        }

                        parent = child;
                }
        }
}

} // namespace NfCore
