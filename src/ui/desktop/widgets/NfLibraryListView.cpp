/**
 * File name: NfLibraryListView.h
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

#include "NfLibraryListView.h"
#include "NfLibraryTreeModel.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QContextMenuEvent>
#include <QMenu>

using namespace NfUi;

namespace NfDesktop {

NfLibraryListView::NfLibraryListView(const NfLibraryContext& ctx,
                                     NfLibraryTreeModel* model,
                                     QWidget* parent)
        : QListView(parent)
        , m_context{ctx}
        , m_model{model}
{
        setEditTriggers(QAbstractItemView::NoEditTriggers);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setUniformItemSizes(true);
        setModel(m_model);

        QObject::connect(m_model, &QAbstractItemModel::rowsInserted,
                         this, [this] { updateGeometry(); });
        QObject::connect(m_model, &QAbstractItemModel::rowsRemoved,
                         this, [this] { updateGeometry(); });
        QObject::connect(m_model, &QAbstractItemModel::modelReset,
                         this, [this] { updateGeometry(); });
}

QSize NfLibraryListView::sizeHint() const
{
        constexpr int maxVisibleRows = 5;
        const auto rowCount = model() ? model()->rowCount(rootIndex()) : 0;
        const auto visibleRows = qMin(maxVisibleRows, rowCount);
        const auto fallbackRowHeight = qMax(fontMetrics().height(),
                                             iconSize().height()) + 4;
        const auto rowHeight = qMax(sizeHintForRow(0), fallbackRowHeight);
        auto hint = QListView::sizeHint();
        hint.setHeight(visibleRows * rowHeight);
        return hint;
}

void NfLibraryListView::contextMenuEvent(QContextMenuEvent* event)
{
        const auto index = indexAt(event->pos());
        if (!index.isValid()) {
                event->ignore();
                return;
        }

        const auto libraryId = m_model->data(index,
                                             NfLibraryTreeModel::LibraryIdRole).toULongLong();
        if (libraryId == 0) {
                event->ignore();
                return;
        }
        setCurrentIndex(index);

        QMenu menu(this);
        menu.setObjectName("LibraryContextMenu");
        auto* renameAction = menu.addAction(tr("Rename Library"));
        auto* removeAction = menu.addAction(tr("Delete Library"));

        const auto* selectedAction = menu.exec(viewport()->mapToGlobal(event->pos()));
        if (selectedAction == renameAction)
                emit renameLibraryRequested(libraryId);
        else if (selectedAction == removeAction)
                emit removeLibraryRequested(libraryId);
}

} // namespace NfDesktop
