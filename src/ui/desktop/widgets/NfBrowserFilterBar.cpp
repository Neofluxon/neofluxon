/**
 * File name: NfBrowserFilterBar.cpp
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

#include "NfBrowserFilterBar.h"

#include <QHBoxLayout>
#include <QMenu>

namespace NfDesktop {

NfBrowserFilterBar::NfBrowserFilterBar(QWidget* parent)
        : NfStyledWidget(parent)
{
        setupUi();
}

void NfBrowserFilterBar::setupUi()
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 2, 0, 2);
        layout->setSpacing(0);

        m_button = new QToolButton(this);
        m_button->setText(tr("Filter (3)"));
        m_button->setPopupMode(QToolButton::InstantPopup);

        auto* menu = new QMenu(m_button);
        menu->addAction(tr("Rated"));
        menu->addAction(tr("Unrated"));
        menu->addSeparator();
        menu->addAction(tr("RAW"));
        menu->addAction(tr("JPEG"));
        menu->addSeparator();
        menu->addAction(tr("Edited"));
        menu->addAction(tr("Unedited"));
        menu->addSeparator();
        menu->addAction(tr("Clear filters"));

        m_button->setMenu(menu);

        layout->addWidget(m_button);
}

} // namespace NfDesktop
