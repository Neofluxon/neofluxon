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

#include "NfLibraryTreeView.h"
#include "NfLibraryTreeModel.h"

#include <QHeaderView>
#include <QAbstractItemModel>
#include <QKeyEvent>

namespace NfDesktop {

NfLibraryTreeView::NfLibraryTreeView(QWidget* parent)
        : QTreeView(parent)
        , m_model{new NfLibraryTreeModel(this)}
{
        setupView();
        setupBehavior();
        setModel(m_model);
}

void NfLibraryTreeView::setupView()
{
        // No horizontal header (clean sidebar look)
        header()->hide();

        // Tree behavior
        setRootIsDecorated(true);     // show expand arrows
        setItemsExpandable(true);
        setExpandsOnDoubleClick(true);

        // Selection
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);

        // Performance
        setUniformRowHeights(true);

        // Nice indentation for hierarchy
        setIndentation(18);
}

void NfLibraryTreeView::setupBehavior()
{
        // Optional: no edit
        setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Optional: keyboard navigation feel like file browser
        setFocusPolicy(Qt::StrongFocus);
}

} // namespace NfDesktop
