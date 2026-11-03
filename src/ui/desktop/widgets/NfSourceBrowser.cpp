/**
 * File name: NfSourceBrowser.h
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

#include "NfSourceBrowser.h"
#include "NfScrollStack.h"
#include "NfCollapsibleSection.h"
#include "NfPathTreeView.h"
#include "NfPlacesListView.h"
#include "NfStorageListView.h"

#include <QVBoxLayout>

namespace NfDesktop {

NfSourceBrowser::NfSourceBrowser(QWidget* parent)
        : QWidget(parent)
        , m_scrollStack{new NfScrollStack(this)}
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        layout->addWidget(m_scrollStack);

        // Folders section
        auto* foldersSection = new NfCollapsibleSection(tr("Folders"));
        foldersSection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_pathTreeView = new NfPathTreeView();
        m_pathTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        foldersSection->setContent(m_pathTreeView);
        m_scrollStack->addWidget(foldersSection);
        QObject::connect(foldersSection,
                         &NfCollapsibleSection::expandedChanged,
                         this,
                         [this, foldersSection](bool expanded) {
                                 m_scrollStack->setWidgetStretch(foldersSection,
                                                                 expanded ? 1 : 0);
                         });

        // Places section
        auto* section = new NfCollapsibleSection(tr("Places"));
        m_placesListView = new NfPlacesListView();
        section->setContent(m_placesListView);
        m_scrollStack->addWidget(section);

        // Storage section
        section = new NfCollapsibleSection(tr("Storage"));
        m_storageListView = new NfStorageListView();
        section->setContent(m_storageListView);
        m_scrollStack->addWidget(section);

        foldersSection->setExpanded(true);

        QObject::connect(m_pathTreeView,
                         &NfPathTreeView::folderSelected,
                         this,
                         &NfSourceBrowser::folderSelected);
        QObject::connect(m_pathTreeView,
                         &NfPathTreeView::importRequested,
                         this,
                         &NfSourceBrowser::importRequested);
        QObject::connect(m_placesListView,
                         &NfPlacesListView::pathSelected,
                         m_pathTreeView,
                         &NfPathTreeView::selectPath);
        QObject::connect(m_storageListView,
                         &NfStorageListView::pathSelected,
                         m_pathTreeView,
                         &NfPathTreeView::selectPath);
}

} // namespace NfDesktop
