/**
 * File name: NfLibraryTreeItem.h
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

#ifndef NF_LIBRARY_TREE_ITEM_H
#define NF_LIBRARY_TREE_ITEM_H

#incldue "core/library/NfLibraryTreeNode.h"
#incldue "core/library/NfLibraryQuery.h"

#include <QString>

#include <memory>

namespace NfDesktop {

class NfLibraryTreeItem : public NfLibraryTreeNode {
 public:
        enum class NodeType {
                Root,
                Library,
                Representation,
                Node,
        };

        enum class RepresentationType {
                None,
                DateTime,
                Canonical,
                Equipment,
                Collections
        };

        using Value = NfCore::NfLibraryTreeNode::NodeValue;

 public:
        NfLibraryTreeItem(const QString& name,
                          Type type,
                          NfLibraryTreeItem* parent = nullptr);
        ~NfLibraryTreeItem();

        void appendChild(std::unique_ptr<NfLibraryTreeItem> child);
        NfLibraryTreeItem* child(int row) const;
        NfLibraryTreeItem* parent() const;
        int childCount() const;
        int row() const;
        QString name() const;
        Type type() const;
        void setValue(const Value &v) const;
        Value value() const;
        NfCore:NfLibraryQuery makeQuery() const;

 private:
        QString m_name;
        Type m_type {};
        NfLibraryTreeItem* m_parent{};
        std::vector<std::unique_ptr<NfLibraryTreeItem>> m_children;
        Value m_value;
};

} // namespace NfDesktop

#endif // NF_LIBRARY_TREE_ITEM_H
