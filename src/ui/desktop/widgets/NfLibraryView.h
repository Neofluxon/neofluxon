/**
 * File name: NfLibraryView.h
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

#ifndef NF_LIBRARY_VIEW_H
#define NF_LIBRARY_VIEW_H

#include "NfDesktopContext.h"
#include "NfLibraryContext.h"

#include <QWidget>

class QVBoxLayout;

namespace NfUi {
 class NfUiLibraryModeState;
}

namespace NfDesktop {

class NfLibraryModel;
class NfBrowserView;

class NfLibraryView : public QWidget
{
        Q_OBJECT

 public:
        explicit NfLibraryView(NfDesktopContext context,
                               QWidget* parent = nullptr);

private:
        NfDesktopContext m_desktopContext;
        NfUi::NfLibraryContext m_context;
        NfUi::NfUiLibraryModeState *m_state;
        NfLibraryModel* m_model;
        QVBoxLayout* m_mainLayout;
        NfBrowserView* m_browserView;
};

} // namespace NfDesktop

#endif // NF_LIBRARY_VIEW_H
