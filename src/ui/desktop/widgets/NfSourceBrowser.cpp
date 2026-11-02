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

namespace NfDesktop {

NfSourceBrowser::NfSourceBrowser(QWidget* parent)
        : QWidget(parent)
        , m_scrollStack{new NfScrollStack(this)}
{
        // Folders section
        auto* section = new NfCollapsibleSection(title);
        section->setContent(new NfPathTreeView());
        m_scrollStack->addWidget(section);

        // Places section
        auto* section = new NfCollapsibleSection(title);
        section->setContent(new NfPlacesListView());
        m_scrollStack->addWidget(section);

        // Storage section
        auto* section = new NfCollapsibleSection(title);
        section->setContent(new NfStorageListView());
        m_scrollStack->addWidget(section);
}

} // namespace NfDesktop
