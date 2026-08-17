/**
 * File name: NfBrowserTopBar.h
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

#ifndef NF_BROWSER_TOPBAR_H
#define NF_BROWSER_TOPBAR_H

#include "NfStyledWidget.h"

namespace NfDesktop {

        //class NfBrowserBreadcrumbBar;
class NfBrowserViewModeBar;
class NfBrowserSortBar;
class NfBrowserFilterBar;

class NfBrowserTopBar : public NfStyledWidget
{
        Q_OBJECT

public:
        explicit NfBrowserTopBar(QWidget* parent = nullptr);
        ~NfBrowserTopBar() override = default;

private:
        void setupUi();

        //        NfBrowserBreadcrumbBar* m_breadcrumbBar{nullptr};
        NfBrowserViewModeBar* m_viewModeBar{nullptr};
        NfBrowserSortBar* m_sortBar{nullptr};
        NfBrowserFilterBar* m_filterBar{nullptr};
};

} // namespace NfDesktop

#endif // NF_BROWSER_TOPBAR_H
