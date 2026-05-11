/**
 * File name: NfLibraryTreeView.h
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

#ifndef NF_LIBRARY_TREE_VIEW_H
#define NF_LIBRARY_TREE_VIEW_H

#include <QTreeView>

namespace NfDesktop {

class NfLibraryTreeModel;

class NfLibraryTreeView : public QTreeView {
    Q_OBJECT

 public:
        explicit NfLibraryTreeView(QWidget* parent = nullptr);
        ~NfLibraryTreeView() override = default;

 protected:
        void setupView();
        void setupBehavior();

 private:
        NfLibraryTreeModel* m_model;
};

} // namespace NfDesktop

#endif // NF_LIBRARY_TREE_VIEW_H
