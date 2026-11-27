/**
 * File name: NfProjectsListView.cpp
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

#include "NfProjectsListView.h"
#include "NfDesktopContext.h"
#include "NfProjectsModel.h"
#include "NfProjectCardDelegate.h"

namespace NfDesktop {

NfProjectsListView::NfProjectsListView(const NfDesktopContext &ctx,
                                       NfProjectsModel* model,
                                       QWidget* parent)
        : QListView(parent)
        , m_context{ctx}
{
        setAttribute(Qt::WA_StyledBackground, true);

        auto delegate = new NfProjectCardDelegate(this);
        setItemDelegate(delegate);

        setModel(model);

        setViewMode(QListView::IconMode);
        setFlow(QListView::LeftToRight);
        setMovement(QListView::Static);
        setUniformItemSizes(true);
        setWrapping(true);
        setResizeMode(QListView::Adjust);
        setMinimumHeight(0);
        setMaximumHeight(QWIDGETSIZE_MAX);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setIconSize({250, 250});
        setGridSize({250, 250});
}

} // namespace NfDesktop
