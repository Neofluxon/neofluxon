/**
 * File name: NfProjectsManager.cpp
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

#include "NfProjectsManager.h"
#include "NfUiState.h"
#include "NfDesktopContext.h"
#include "NfNewProjectDialog.h"
#include "NfUiProjectsModeState.h"

#include <QWidget>

using namespace NfUi;

namespace NfDesktop {

NfProjectsManager::NfProjectsManager(const NfDesktopContext &ctx,
                                     QWidget* parent)
        : QObject(parent)
        , m_parentWidget{parent}
        , m_context{ctx}
{
        QObject::connect(m_context.uiState()->projectsModeState(),
                         &NfUiProjectsModeState::addNewProject,
                         this,
                         &NfProjectsManager::addNewProject);
}

void NfProjectsManager::addNewProject()
{
        if (!m_parentWidget)
                return;

        NfNewProjectDialog dialog(*m_context.uiContext, m_parentWidget);
        dialog.exec();
}

} // namespace NfDesktop
