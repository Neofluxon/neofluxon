/**
 * File name: NfLibraryTreeSnapshot.h
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef NF_LIBRARY_TREE_SNAPSHOT_H
#define NF_LIBRARY_TREE_SNAPSHOT_H

#include "NfLibraryRepresentation.h"

#include <cstdint>
#include <string>
#include <vector>

namespace NfCore {

struct NfLibraryTreeNodeSnapshot {
        std::string name;
        NfLibraryTreeNode::NodeType type;
        NfLibraryTreeNode::NodeValue value;
        std::vector<NfLibraryTreeNodeSnapshot> children;
};

struct NfLibraryRepresentationSnapshot {
        std::string name;
        NfLibraryRepresentation::RepresentationType type;
        std::vector<NfLibraryTreeNodeSnapshot> children;
};

struct NfLibrarySnapshot {
        int64_t id;
        std::string name;
        std::vector<NfLibraryRepresentationSnapshot> representations;
};

} // namespace NfCore

#endif // NF_LIBRARY_TREE_SNAPSHOT_H
