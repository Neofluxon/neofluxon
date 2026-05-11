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

namespace NfDesktop {

NfLibraryTreeModel::NfLibraryTreeModel(QObject* parent)
        : QAbstractItemModel(parent)
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
        m_root = std::make_unique<NfLibraryTreeItem>("", NfLibraryTreeItem::Type::Node);

        // Library
        auto library = std::make_unique<NfLibraryTreeItem>(tr("My Library"),
                                                           NfLibraryTreeItem::Type::Library,
                                                           m_root.get());

        // DateTime
        auto datetime = std::make_unique<NfLibraryTreeItem>(tr("DateTime"),
                                                            NfLibraryTreeItem::Type::Representation,
                                                            library.get());
        datetime->setRepresentationType(NfLibraryTreeItem::RepresentationType::DateTime);
        datetime->appendChild(std::make_unique<NfLibraryTreeItem>("2026",
                                                                  NfLibraryTreeItem::Type::Node,
                                                                datetime.get()));

        // Canonical
        auto canonical = std::make_unique<NfLibraryTreeItem>(tr("Canonical"),
                                                             NfLibraryTreeItem::Type::Representation,
                                                             library.get());
        canonical->setRepresentationType(NfLibraryTreeItem::RepresentationType::Canonical);
        canonical->appendChild(std::make_unique<NfLibraryTreeItem>(
                                                                   "/Photos/Wedding",
                                                                   NfLibraryTreeItem::Type::Node,
                                                                   canonical.get()));
        // Equipment
        auto equipment = std::make_unique<NfLibraryTreeItem>(tr("Equipment"),
                                                             NfLibraryTreeItem::Type::Representation,
                                                             library.get());
        equipment->setRepresentationType(NfLibraryTreeItem::RepresentationType::Equipment);
        equipment->appendChild(std::make_unique<NfLibraryTreeItem>(
                                                        "Canon EOS R6",
                                                        NfLibraryTreeItem::Type::Node,
                                                        equipment.get()));

        // Collections
        auto collections = std::make_unique<NfLibraryTreeItem>(tr("Collections"),
                                                               NfLibraryTreeItem::Type::Representation,
                                                               library.get());
        collections->setRepresentationType(NfLibraryTreeItem::RepresentationType::Collections);
        collections->appendChild(std::make_unique<NfLibraryTreeItem>("Favorites",
                                                                     NfLibraryTreeItem::Type::Node,
                                                                     collections.get()));

        library->appendChild(std::move(datetime));
        library->appendChild(std::move(canonical));
        library->appendChild(std::move(equipment));
        library->appendChild(std::move(collections));
        m_root->appendChild(std::move(library));
}

} // namespace NfDesktop
