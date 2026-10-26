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
#include "NfLibraryTreeSnapshot.h"
#include "NfLibraryContext.h"
#include "NfLibraryAdapter.h"
#include "core/library/NfLibrary.h"
#include "core/NfLogger.h"

#include <QIcon>
#include <QMetaObject>
#include <QPointer>

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfLibraryTreeModel::NfLibraryTreeModel(const NfUi::NfLibraryContext& ctx, QObject* parent)
        : QAbstractItemModel(parent)
        , m_library{ctx.library()}
{
        m_root = std::make_unique<NfLibraryTreeItem>(tr("Libraries"),
                                                     NfLibraryTreeItem::NodeType::Root);

        QObject::connect(m_library,
                         &NfLibraryAdapter::libraryTreeUpdated,
                         this,
                         &NfLibraryTreeModel::onTreeUpdated);
        QObject::connect(m_library,
                         &NfLibraryAdapter::libraryBranchUpdated,
                         this,
                         &NfLibraryTreeModel::onLibraryBranchUpdated);
        updateTree();
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
        if (!item)
                return {};


        switch (role) {
        case Role::LibraryIdRole:
                if (item->type() == NfLibraryTreeItem::NodeType::Library) {
                        const auto value = item->value();
                        if (const auto* libraryId = std::get_if<int64_t>(&value))
                                return QVariant::fromValue<qulonglong>(static_cast<qulonglong>(*libraryId));
                }
                break;
        case Qt::DisplayRole:
                return item->name();

        case Qt::DecorationRole: {
                if (item->type() == NfLibraryTreeItem::NodeType::Representation) {
                        // Safely extract the int value from std::variant
                        auto val = item->value();
                        if (const auto* valPtr = std::get_if<int>(&val)) {
                                auto repType = static_cast<NfLibraryTreeItem::RepresentationType>(*valPtr);

                                switch (repType) {
                                case NfLibraryTreeItem::RepresentationType::Canonical:
                                        return QIcon(":/icons/folder.svg");

                                case NfLibraryTreeItem::RepresentationType::DateTime:
                                        return QIcon(":/icons/calendar.svg");

                                case NfLibraryTreeItem::RepresentationType::Equipment:
                                        return QIcon(":/icons/equipment.svg");

                                case NfLibraryTreeItem::RepresentationType::Collections:
                                        return QIcon(":/icons/collections.svg");

                                default:
                                        break;
                                }
                        }
                }
                break;
        }

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

bool NfLibraryTreeModel::addLibrary(const QString &name)
{
        auto library = m_library->addLibrary(name.toStdString());
        if (library && *library) {
                m_library->requestLibraryBranch((*library)->id());
                return true;
        }

        return false;
}

bool NfLibraryTreeModel::updateLibraryName(uint64_t libraryId, const QString& name)
{
        const auto row = libraryRow(libraryId);
        if (row < 0 || !m_library->updateLibrary(libraryId, name.toStdString()))
                return false;

        auto* item = m_root->child(row);
        item->setName(name);
        const auto changedIndex = index(row, 0);
        emit dataChanged(changedIndex, changedIndex, {Qt::DisplayRole});
        return true;
}

bool NfLibraryTreeModel::removeLibrary(uint64_t libraryId)
{
        // Find the library row so only its branch needs to be removed.
        const auto row = libraryRow(libraryId);
        if (row < 0)
                return false;

        if (!m_library->removeLibrary(libraryId))
                return false;

        // Notify views before changing the model's backing tree.
        beginRemoveRows(QModelIndex(), row, row);
        m_root->removeChild(row);
        endRemoveRows();
        return true;
}

int NfLibraryTreeModel::libraryRow(uint64_t libraryId) const
{
        for (int row = 0; row < m_root->childCount(); ++row) {
                const auto value = m_root->child(row)->value();
                if (const auto* id = std::get_if<int64_t>(&value);
                    id && static_cast<uint64_t>(*id) == libraryId)
                        return row;
        }

        return -1;
}

void NfLibraryTreeModel::updateTree()
{
        m_library->requestLibraryTree();
}

void NfLibraryTreeModel::onTreeUpdated(const NfLibraryTreeSnapshot& snapshot)
{
        beginResetModel();
        m_root = std::make_unique<NfLibraryTreeItem>(
                tr("Libraries"), NfLibraryTreeItem::NodeType::Root);
        for (const auto& library : snapshot)
                addLibrarySnapshot(library);
        endResetModel();
        emit treeLoaded();
}

void NfLibraryTreeModel::onLibraryBranchUpdated(const NfLibrarySnapshot& snapshot)
{
        const auto row = libraryRow(snapshot.id);
        if (row >= 0)
                return;

        const auto insertRow = m_root->childCount();
        beginInsertRows(QModelIndex(), insertRow, insertRow);
        addLibrarySnapshot(snapshot);
        endInsertRows();
        emit treeLoaded();
}

NfLibraryTreeItem* NfLibraryTreeModel::itemFromIndex(const QModelIndex& index) const
{
        if (index.isValid())
                return static_cast<NfLibraryTreeItem*>(index.internalPointer());

        return m_root.get();
}

void NfLibraryTreeModel::addLibrarySnapshot(const NfLibrarySnapshot& library)
{
        auto name = QString::fromUtf8(library.name.c_str());
        auto nodeType = NfLibraryTreeItem::NodeType::Library;
        auto libraryItem = std::make_unique<NfLibraryTreeItem>(name,
                                                               nodeType,
                                                               m_root.get());
        libraryItem->setValue(library.id);
        for (const auto& representation : library.representations) {
                auto repItem = std::make_unique<NfLibraryTreeItem>(
                        QString::fromUtf8(representation.name.c_str()),
                        NfLibraryTreeItem::NodeType::Representation,
                        libraryItem.get());
                repItem->setValue(static_cast<int>(representation.type));
                populateChildNodes(representation.children, repItem.get());
                libraryItem->appendChild(std::move(repItem));
        }
        m_root->appendChild(std::move(libraryItem));
}

void NfLibraryTreeModel::populateChildNodes(const std::vector<NfLibraryTreeNodeSnapshot>& children,
                                            NfLibraryTreeItem* parentItem)
{
        for (const auto& child : children) {
                auto nodeType = NfLibraryTreeItem::NodeType::Node;
                auto name = QString::fromStdString(child.name);
                auto node = std::make_unique<NfLibraryTreeItem>(name,
                                                                nodeType,
                                                                parentItem);
                auto value = child.value;
                if (child.type == NfLibraryTreeNode::NodeType::Camera
                    || child.type == NfLibraryTreeNode::NodeType::Lens) {
                        if (const auto* id = std::get_if<int64_t>(&value)) {
                                const auto kind = child.type == NfLibraryTreeNode::NodeType::Camera
                                        ? NfEquipmentQuery::Kind::Camera
                                        : NfEquipmentQuery::Kind::Lens;
                                value = NfEquipmentQuery{kind, *id};
                        }
                }
                node->setValue(value);
                if (!child.children.empty())
                        populateChildNodes(child.children, node.get());

                parentItem->appendChild(std::move(node));
        }
}

} // namespace NfDesktop
