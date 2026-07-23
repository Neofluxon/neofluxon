/**
 * File name: NfLeftPanel.h
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

#ifndef NF_LEFT_PANEL_H
#define NF_LEFT_PANEL_H

#include "NfPanel.h"

#include <filesystem>

class QStackedWidget;

namespace NfUi {
struct NfContext;
}

using namespace NfUi;

namespace NfDesktop {

class NfPathBrowser;
class NfLibraryTreeView;
class NfLibraryBrowser;

class NfLeftPanel : public NfPanel {
  Q_OBJECT

public:
        explicit NfLeftPanel(NfContext *ctx, QWidget *parent = nullptr);
        virtual ~NfLeftPanel() = default;

protected slots:
        void showLibraryBrowser();
        void showPathBrowser();
        void showImportWidget(const std::filesystem::path &path);

private:
        NfContext *m_context;
        NfPathBrowser* m_pathBrowser;
        NfLibraryBrowser* m_libraryBrowser;
        QStackedWidget* m_stack;
};

} // namespace NfDesktop

#endif // NF_LEFT_PANEL_H
