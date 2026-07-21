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
#include "NfLibraryContext.h"
#include "NfLibraryAdapter.h"
#include "core/library/NfLibraryManager.h"
#include "core/library/NfLibrary.h"
#include "core/library/NfLibraryRepresentation.h"
#include "core/NfLogger.h"
#include "core/library/NfLibraryTreeNode.h"

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfLibraryTreeModel::NfLibraryTreeModel(const NfUi::NfLibraryContext& ctx, QObject* parent)
        : QAbstractItemModel(parent)
        , m_library{ctx.library()}
{
        buildTree();

        QObject::connect(m_library,
                         &NfLibraryAdapter::folderImported,
                         this,
                         &NfLibraryTreeModel::updateTree);
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

void NfLibraryTreeModel::updateTree()
{
        beginResetModel();
        buildTree();
        endResetModel();
}

NfLibraryTreeItem* NfLibraryTreeModel::itemFromIndex(const QModelIndex& index) const
{
        if (index.isValid())
                return static_cast<NfLibraryTreeItem*>(index.internalPointer());

        return m_root.get();
}

void NfLibraryTreeModel::buildTree()
{
        auto nodeType = NfLibraryTreeItem::NodeType::Root;
        m_root = std::make_unique<NfLibraryTreeItem>("Libraries", nodeType);

        const auto libraries = m_library->libraries();
        for (const auto& library : libraries) {

                auto name = QString::fromUtf8(library->name().c_str());
                nodeType = NfLibraryTreeItem::NodeType::Library;
                auto libraryItem = std::make_unique<NfLibraryTreeItem>(name,
                                                                       nodeType,
                                                                       m_root.get());
                populateLibrary(library.get(), libraryItem.get());

                m_root->appendChild(std::move(libraryItem));
        }
}

void NfLibraryTreeModel::populateLibrary(NfLibrary* library,
                                         NfLibraryTreeItem* libraryItem)
{
        libraryItem->setValue(library->id());
        for (const auto& rep : library->representations()) {
                auto name = QString::fromUtf8(rep->name().c_str());
                auto nodeType = NfLibraryTreeItem::NodeType::Representation;
                auto repItem = std::make_unique<NfLibraryTreeItem>(name,
                                                                   nodeType,
                                                                   libraryItem);
                populateRepresentation(rep.get(), repItem.get());
                libraryItem->appendChild(std::move(repItem));
        }
}

void NfLibraryTreeModel::populateRepresentation(NfLibraryRepresentation* rep,
                                                NfLibraryTreeItem* parent)
{
        parent->setValue(static_cast<int>(rep->type()));

        auto* repTree = rep->getTree();
        if (repTree)
                populateChildNodes(repTree->children(), parent);
}

void NfLibraryTreeModel::populateChildNodes(const std::vector<std::unique_ptr<NfLibraryTreeNode>>& children,
                                            NfLibraryTreeItem* parentItem)
{
        for (const auto& child : children) {
                auto nodeType = NfLibraryTreeItem::NodeType::Node;
                auto name = QString::fromStdString(child->name());
                auto node = std::make_unique<NfLibraryTreeItem>(name,
                                                                nodeType,
                                                                parentItem);

                const auto value = child->getValue();
                if (const auto* val = std::get_if<int64_t>(&value))
                        node->setValue(child->getValue());

                if (!child->children().empty())
                        populateChildNodes(child->children(), node.get());

                parentItem->appendChild(std::move(node));
        }
}

} // namespace NfDesktop
