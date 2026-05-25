/**
 * File name: NfLibraryTreeModel.h
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

#include "NfLibraryTreeModel.h"
#include "NfLibraryTreeItem.h"
#include "NfContext.h"
#include "core/NfLibraryManager.h"
#include "core/NfLibrary.h"
#include "core/NfLibraryRepresentation.h"
#include "core/NfLibraryTreeNode.h"

using namespace NfUi;

namespace NfDesktop {

NfLibraryTreeModel::NfLibraryTreeModel(const NfContext& ctx, QObject* parent)
        : QAbstractItemModel(parent)
        , m_library{ctx->library}
{
        buildTree();
}

NfLibraryTreeModel::~NfLibraryTreeModel() = default;

QModelIndex NfLibraryTreeModel::index(int row,
                                      int column,
                                      const QModelIndex& parent) const
{
        if (!hasIndex(row, column, parent))
                return QModelIndex();

        auto* parentItem = itemFromIndex(parent);
        auto* childItem = parentItem->child(row);
        if (!childItem)
                return QModelIndex();

        return createIndex(row, column, childItem);
}

QModelIndex NfLibraryTreeModel::parent(const QModelIndex& index) const
{
        if (!index.isValid())
                return QModelIndex();

        auto* childItem = static_cast<NfLibraryTreeItem*>(index.internalPointer());
        auto* parentItem = childItem->parent();
        if (!parentItem || parentItem == m_root.get())
                return QModelIndex();

        return createIndex(parentItem->row(),
                           0,
                           parentItem);
}

int NfLibraryTreeModel::rowCount(const QModelIndex& parent) const
{
        auto* parentItem = itemFromIndex(parent);
        return parentItem->childCount();
}

int NfLibraryTreeModel::columnCount(const QModelIndex&) const
{
        return 1;
}

QVariant NfLibraryTreeModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid())
                return {};

        auto* item = static_cast<NfLibraryTreeItem*>(index.internalPointer());

        switch (role) {
        case Qt::DisplayRole:
                return item->name();
        default:
                break;
        }

        return {};
}

Qt::ItemFlags NfLibraryTreeModel::flags(const QModelIndex& index) const
{
        if (!index.isValid())
                return Qt::NoItemFlags;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

NfLibraryTreeItem* NfLibraryTreeModel::itemFromIndex(const QModelIndex& index) const
{
        if (index.isValid())
                return static_cast<NfLibraryTreeItem*>(index.internalPointer());

        return m_root.get();
}

void NfLibraryTreeModel::buildTree()
{
        /*m_root = std::make_unique<NfLibraryTreeItem>("",
                                                     NfLibraryTreeItem::Type::Node);

        const auto libraries = m_library->libraries();

        for (const auto& library : libraries) {

                auto libraryItem =
                        populateLibrary(library, m_root.get());

                m_root->appendChild(std::move(libraryItem));
                }*/
}

std::unique_ptr<NfLibraryTreeItem>
NfLibraryTreeModel::populateLibrary(NfLibrary* library,
                                    NfLibraryTreeItem* parent)
{
        auto libraryItem = std::make_unique<NfLibraryTreeItem>(library->name(),
                                                               NfLibraryTreeItem::Type::Library,
                                                               parent);

        for (const auto& rep : library->representations()) {
                auto repItem = populateRepresentation(rep, libraryItem.get());
                libraryItem->appendChild(std::move(repItem));
        }

        return libraryItem;
}

std::unique_ptr<NfLibraryTreeItem>
NfLibraryTreeModel::populateRepresentation(NfLibraryRepresentation* rep,
                                           NfLibraryTreeItem* parent)
{
        auto repItem = std::make_unique<NfLibraryTreeItem>(rep->name(),
                                                           NfLibraryTreeItem::Type::Representation,
                                                           parent);
        populateChildNodes(rep->getTree()->children(),
                           repItem.get());
        return repItem;
}

void NfLibraryTreeModel::populateChildNodes(const std::vector<NfLibraryTreeNode*>& children,
                                            NfLibraryTreeItem* parentItem)
{
        for (const auto* child : children) {
                auto node = std::make_unique<NfLibraryTreeItem>(child->name(),
                                                                NfLibraryTreeItem::Type::Node,
                                                                parentItem);
                if (!child->children().empty())
                        populateChildNodes(child->children(), node.get());
                parentItem->appendChild(std::move(node));
        }
}

} // namespace NfDesktop
