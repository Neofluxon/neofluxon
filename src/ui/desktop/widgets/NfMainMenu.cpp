/**
 * File name: NfMainMenu.cpp
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

#include "NfMainMenu.h"

#include <QIcon>
#include <QSize>
#include <QVBoxLayout>
#include <QPushButton>

namespace NfDesktop {

NfMainMenu::NfMainMenu(QWidget* parent)
        : NfStyledWidget(parent)
        , shootsButton{nullptr}
        , foldersButton{nullptr}
        , libraryButton{nullptr}

{
        setObjectName("NfMainMenu");

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        auto menuLayout = new QVBoxLayout(this);
        menuLayout->setContentsMargins(6, 6, 6, 6);
        menuLayout->setSpacing(2);

        shootsButton = new QPushButton(tr("Shoots"), this);
        shootsButton->setIcon(QIcon(":/icons/nav-shoots.svg"));
        shootsButton->setIconSize(QSize(16, 16));
        shootsButton->setCheckable(true);
        shootsButton->setAutoExclusive(true);
        QObject::connect(shootsButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::shootsClicked);
        menuLayout->addWidget(shootsButton);

        foldersButton = new QPushButton(tr("Folders"), this);
        foldersButton->setIcon(QIcon(":/icons/nav-folders.svg"));
        foldersButton->setIconSize(QSize(16, 16));
        foldersButton->setCheckable(true);
        foldersButton->setAutoExclusive(true);
        QObject::connect(foldersButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::foldersClicked);
        menuLayout->addWidget(foldersButton);

        libraryButton = new QPushButton(tr("Library"), this);
        libraryButton->setIcon(QIcon(":/icons/nav-library.svg"));
        libraryButton->setIconSize(QSize(16, 16));
        libraryButton->setCheckable(true);
        libraryButton->setAutoExclusive(true);
        QObject::connect(libraryButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::libraryClicked);
        menuLayout->addWidget(libraryButton);

        menuLayout->addStretch();

        setLayout(menuLayout);
}

void NfMainMenu::setShootsMode()
{
        shootsButton->setChecked(true);
        foldersButton->setChecked(false);
        libraryButton->setChecked(false);
}

void NfMainMenu::setFolderMode()
{
        shootsButton->setChecked(false);
        foldersButton->setChecked(true);
        libraryButton->setChecked(false);
}

void NfMainMenu::setLibraryMode()
{
        shootsButton->setChecked(false);
        foldersButton->setChecked(false);
        libraryButton->setChecked(true);
}

} // namespace NfDesktop
