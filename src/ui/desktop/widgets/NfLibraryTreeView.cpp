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
#include "NfContext.h"

#include <QHeaderView>
#include <QAbstractItemModel>
#include <QKeyEvent>

using namespace NfUi;

namespace NfDesktop {

NfLibraryTreeView::NfLibraryTreeView(const NfContext& ctx, QWidget* parent)
        : QTreeView(parent)
        , m_model{new NfLibraryTreeModel(ctx, this)}
        , m_state{cts.uiState->libraryModeState()}
{
        setupView();
        setupBehavior();
        setModel(m_model);
}

void NfLibraryTreeView::setupView()
{
        header()->hide();

        setRootIsDecorated(true);
        setItemsExpandable(true);
        setExpandsOnDoubleClick(true);

        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);

        setUniformRowHeights(true);
        setIndentation(18);
}

void NfLibraryTreeView::setupBehavior()
{
        setEditTriggers(QAbstractItemView::NoEditTriggers);
        setFocusPolicy(Qt::StrongFocus);
}

NfLibraryTreeView::currentChanged(const QModelIndex& current,
                                  const QModelIndex& previous)
{
        if (auto* node = m_model->itemFromIndex(current); node)
                m_state.queryLibrary(node->makeQuiery());
}

} // namespace NfDesktop
