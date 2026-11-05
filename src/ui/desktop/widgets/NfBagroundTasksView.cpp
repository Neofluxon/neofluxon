/**
 * File name: NfBagroundTasksView.cpp
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
 * This program is distributed in the hope that it will be useful;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "NfBagroundTasksView.h"
#include "NfBagroundTaskDelegate.h"
#include "NfBagroundTasksModel.h"

namespace NfDesktop {

NfBagroundTasksView::NfBagroundTasksView(NfBagroundTasksModel* model,
                                         QWidget* parent)
        : QListView(parent)
{
        constexpr int rowHeight = 104;
        constexpr int rowSpacing = 8;
        constexpr int maxVisibleRows = 5;

        setObjectName(QStringLiteral("NfBagroundTasksView"));
        setModel(model);
        setItemDelegate(new NfBagroundTaskDelegate(this));
        setFrameShape(QFrame::NoFrame);
        setSelectionMode(QAbstractItemView::NoSelection);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setSpacing(rowSpacing);
        setUniformItemSizes(true);

        const auto visibleRows = qMin(model->rowCount(), maxVisibleRows);
        setFixedHeight(visibleRows * rowHeight
                       + qMax(0, visibleRows - 1) * rowSpacing);
}

} // namespace NfDesktop
