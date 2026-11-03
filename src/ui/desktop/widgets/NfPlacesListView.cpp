/**
 * File name: NfPlacesListView.h
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

#include "NfPlacesListView.h"

#include <QListWidget>
#include <QVBoxLayout>
#include <QtGlobal>

namespace NfDesktop {

NfPlacesListView::NfPlacesListView(QWidget* parent)
        : QWidget(parent)
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* places = new QListWidget(this);
#if defined(Q_OS_WIN)
        places->addItems({tr("User"),
                          tr("Desktop"),
                          tr("Downloads"),
                          tr("Pictures"),
                          tr("Recycle Bin")});
#elif defined(Q_OS_LINUX)
        places->addItems({tr("/"),
                          tr("Home"),
                          tr("Desktop"),
                          tr("Downloads"),
                          tr("Pictures"),
                          tr("Trash")});
#endif
        layout->addWidget(places);
}

} // namespace NfDesktop
