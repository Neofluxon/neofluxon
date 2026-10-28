/**
 * File name: NfBrowserSortBar.h
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

#ifndef NF_BROWSER_SORTBAR_H
#define NF_BROWSER_SORTBAR_H

#include "NfStyledWidget.h"
#include "NfUiBrowserState.h"

class QAction;
class QToolButton;

namespace NfDesktop {

class NfBrowserSortBar : public NfStyledWidget
{
        Q_OBJECT

        using SortField = NfUi::NfUiBrowserState::SortField;
        using SortOrder = NfUi::NfUiBrowserState::SortOrder;

public:
        explicit NfBrowserSortBar(NfUi::NfUiBrowserState *state,
                                  QWidget* parent = nullptr);
        ~NfBrowserSortBar() override = default;

private:
        void setupUi();
        void updateSorting();

        NfUi::NfUiBrowserState *m_state;
        QToolButton* m_button{nullptr};
        std::unordered_map<SortField, QAction*> m_fieldActions;
        QAction* m_ascendingAction{nullptr};
        QAction* m_descendingAction{nullptr};
};

} // namespace NfDesktop

#endif // NF_BROWSER_SORTBAR_H

