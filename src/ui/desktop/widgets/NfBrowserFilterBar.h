/**
 * File name: NfBrowserFilterBar.h
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

#ifndef NF_BROWSER_FILTERBAR_H
#define NF_BROWSER_FILTERBAR_H

#include "NfStyledWidget.h"
#include "NfUiBrowserState.h"

#include <QToolButton>
#include <utility>
#include <vector>

class QAction;

namespace NfDesktop {

class NfBrowserFilterBar : public NfStyledWidget
{
        Q_OBJECT

public:
        explicit NfBrowserFilterBar(NfUi::NfUiBrowserState* state,
                                    QWidget* parent = nullptr);
        ~NfBrowserFilterBar() override = default;

private:
        void setupUi();
        void updateFormatFilter();

        NfUi::NfUiBrowserState* m_state;
        QToolButton* m_button{nullptr};
        std::vector<std::pair<NfCore::NfPhoto::PhotoFormat, QAction*>> m_formatActions;
        QAction* m_clearAction{nullptr};
};

} // namespace NfDesktop

#endif // NF_BROWSER_FILTERBAR_H
