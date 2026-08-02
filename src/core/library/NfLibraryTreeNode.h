/**
 * File name: NfLibraryTreeNode.h
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

#ifndef NF_LIBRARY_TREE_NODE_H
#define NF_LIBRARY_TREE_NODE_H

#include "NfTimeUtils.h"

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <utility>
#include <map>

namespace NfCore {

class NfLibraryTreeNode {
public:
        enum class NodeType {
                Unknown,

                // Structural
                Root,
                Folder,         // Physical disk location
                Collection,     // Virtual grouping (user-defined)

                // Temporal
                DateYear,
                DateMonth,
                DateDay,

                // Hardware Identity
                Camera,
                Lens,

                // Geographic Hierarchy
                LocationCountry,
                LocationRegion,
                LocationCity,
                // Specific parks, monuments, etc.
                LocationPointOfInterest,

                // People & Metadata
                Person,
                Keyword
        };

        using NodeValue = std::variant<std::monostate,
                                       int,
                                       int64_t,
                                       std::string,
                                       NfDateRange>;

        NfLibraryTreeNode(const std::string& name = {}, NodeType t  = {});
        ~NfLibraryTreeNode();
        void setName(const std::string& name);
        const std::string& name() const;
        void setType(NodeType t);
        NodeType type() const;
        void setValue(const NodeValue& val);
        NodeValue getValue() const;
        NfLibraryTreeNode* addChild();
        void removeChild(NfLibraryTreeNode* child);
        const std::vector<std::unique_ptr<NfLibraryTreeNode>>& children() const;

private:
        NodeType m_type;
        std::string m_name;
        NodeValue m_value;
        std::vector<std::unique_ptr<NfLibraryTreeNode>> m_children;
};

} // namespace NfCore

#endif // NF_LIBRARY_TREE_NODE_H
