/**
 * File name: NfSearchBar.cpp
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

#include "NfSearchBar.h"
#include "NfDesktopContext.h"
#include "NfUiState.h"
#include "NfUiFolderModeState.h"
#include "NfUiLibraryModeState.h"
#include "NfLibraryTreeModel.h"
#include "core/library/NfLibraryQuery.h"
#include "NfSearchCriteriaWidget.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QToolButton>

using namespace NfUi;
using namespace NfCore;

namespace NfDesktop {

NfSearchBar::NfSearchBar(NfDesktopContext &ctx,
                         QWidget* parent)
        : NfStyledWidget(parent)
        , m_state{ctx.uiState()}
{
        setObjectName("NfSearchBar");
        setFixedWidth(220);

        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(4, 0, 4, 0);
        layout->setSpacing(2);

        m_searchTextInput = new QLineEdit(this);
        m_searchTextInput->setObjectName("SearchTextInput");
        m_searchTextInput->setPlaceholderText(tr("Search photos..."));
        m_searchTextInput->setFixedHeight(32);
        layout->addWidget(m_searchTextInput, 1);

        m_searchTextInput->addAction(QIcon(":/icons/search.svg"),
                                     QLineEdit::LeadingPosition);

        QObject::connect(m_searchTextInput,
                         &QLineEdit::returnPressed,
                         this,
                         &NfSearchBar::onSearchRequested);

        auto criteriaButton = new QToolButton(this);
        criteriaButton->setObjectName("SearchCriteriaButton");
        criteriaButton->setToolTip(tr("Select search criteria"));
        criteriaButton->setFixedSize(24, 24);
        criteriaButton->setIcon(QIcon(":/icons/chevron-down.svg"));
        criteriaButton->setIconSize(QSize(16, 16));
        criteriaButton->setAutoRaise(true);
        layout->addWidget(criteriaButton);

        QObject::connect(criteriaButton,
                         &QToolButton::clicked,
                         this,
                         &NfSearchBar::onShowCriteriaPopup);
}

void NfSearchBar::onShowCriteriaPopup()
{
}

void NfSearchBar::onSearchRequested()
{
        switch (m_state->mode()) {
        case NfUiMode::Folders:
                return;
        case NfUiMode::Library:
        {
                auto* state = m_state->libraryModeState();
                if (state) {
                        NfLibraryQuery searchQuery;
                        searchQuery.libraryId = state->currentLibrary();
                        searchQuery.searchText =
                                m_searchTextInput->text().trimmed().toStdString();
                        searchQuery.criteria = m_searchCriteria;
                        state->setQuery(searchQuery);
                }
                return;
        }
        case NfUi::NfUiMode::Shoots:
                return;
        }
}

} // namespace NfDesktop

