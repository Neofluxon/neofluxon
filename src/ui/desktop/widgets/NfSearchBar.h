/**
 * File name: NfSearchBar.h
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

#ifndef NF_SEARCHBAR_H
#define NF_SEARCHBAR_H

#include "NfStyledWidget.h"
#include "core/library/NfLibraryQuery.h"

#include <vector>

class QLineEdit;
class QToolButton;

namespace NfUi {
class NfUiState;
}

namespace NfDesktop {

class NfDesktopContext;
class NfLibraryTreeModel;
class NfSearchCriteriaWidget;

class NfSearchBar : public NfStyledWidget
{
        Q_OBJECT

public:
        explicit NfSearchBar(NfDesktopContext &ctx,
                             QWidget* parent = nullptr);
        ~NfSearchBar() override = default;

protected slots:
        void onSearchRequested();
        void onShowCriteriaPopup();

private:
        NfUi::NfUiState *m_state;
        NfLibraryTreeModel* m_libraryTreeModel{nullptr};
        QLineEdit* m_searchTextInput{nullptr};
        QToolButton* m_criteriaButton{nullptr};
        NfSearchCriteriaWidget* m_criteriaPopup{nullptr};
        std::vector<NfCore::NfLibrarySearchCriterion> m_searchCriteria;
};

} // namespace NfDesktop

#endif // NF_SEARCHBAR_H
