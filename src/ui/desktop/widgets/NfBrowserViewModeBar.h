/**
 * File name: NfBrowserViewModeBar.h
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

#ifndef NF_BROWSER_VIEWMODEBAR_H
#define NF_BROWSER_VIEWMODEBAR_H

#include "NfUiBrowserState.h"
#include "NfStyledWidget.h"

#include <QMap>

class QButtonGroup;
class QToolButton;

namespace NfUi {
class NfUiBrowserState;
}

namespace NfDesktop {

class NfBrowserViewModeBar : public NfStyledWidget
{
    Q_OBJECT

public:
        using ViewMode = NfUi::NfUiBrowserState::ViewMode;
        explicit NfBrowserViewModeBar(QWidget* parent = nullptr);
        ~NfBrowserViewModeBar() override = default;
        NfUi::NfUiBrowserState* getState() const;

public slots:
        void setState(NfUi::NfUiBrowserState *state);

private:
        void setupUi();
        void setupStyle();
        void createModeButton(ViewMode mode,
                              const QString& text,
                              const QIcon& icon = QIcon());

        NfUi::NfUiBrowserState* m_state{nullptr};
        QButtonGroup* m_buttonGroup{nullptr};
        QMap<ViewMode, QToolButton*> m_buttons;
};

} // namespace NfDesktop

#endif // NF_BROWSER_VIEWMODEBAR_H
