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

#ifndef NF_LIBRARY_TREE_MODEL_H
#define NF_LIBRARY_TREE_MODEL_H

#include <QAbstractItemModel>

#include <memory>

namespace NfDesktop {

class NfLibraryTreeItem;

class NfLibraryTreeModel : public QAbstractItemModel {
        Q_OBJECT

 public:
        explicit NfLibraryTreeModel(QObject* parent = nullptr);
        ~NfLibraryTreeModel() override;

 public:
        QModelIndex index(int row,
                          int column,
                          const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

 private:
        NfLibraryTreeItem* itemFromIndex(const QModelIndex& index) const;
        void buildTree();

        std::unique_ptr<NfLibraryTreeItem> m_root;
};

} // namespace NfDesktop

#endif // NF_LIBRARY_TREE_MODEL_H
