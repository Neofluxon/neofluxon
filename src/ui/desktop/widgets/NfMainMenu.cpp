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
#include <QKeySequence>
#include <QShortcut>
#include <QSize>
#include <QVBoxLayout>
#include <QPushButton>

namespace NfDesktop {

NfMainMenu::NfMainMenu(QWidget* parent)
        : NfStyledWidget(parent)
        , m_projectsButton{nullptr}
        , m_foldersButton{nullptr}
        , m_libraryButton{nullptr}

{
        setObjectName("NfMainMenu");

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        auto menuLayout = new QVBoxLayout(this);
        menuLayout->setContentsMargins(6, 6, 6, 6);
        menuLayout->setSpacing(2);

        m_projectsButton = new QPushButton(tr("Projects"), this);
        m_projectsButton->setIcon(QIcon(":/icons/nav-projects.svg"));
        m_projectsButton->setIconSize(QSize(16, 16));
        m_projectsButton->setCheckable(true);
        m_projectsButton->setAutoExclusive(true);
        QObject::connect(m_projectsButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::projectsClicked);
                         menuLayout->addWidget(m_projectsButton);

        m_foldersButton = new QPushButton(tr("Folders"), this);
        m_foldersButton->setIcon(QIcon(":/icons/nav-folders.svg"));
        m_foldersButton->setIconSize(QSize(16, 16));
        m_foldersButton->setCheckable(true);
        m_foldersButton->setAutoExclusive(true);
        QObject::connect(m_foldersButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::foldersClicked);
        menuLayout->addWidget(m_foldersButton);

        m_libraryButton = new QPushButton(tr("Library"), this);
        m_libraryButton->setIcon(QIcon(":/icons/library.svg"));
        m_libraryButton->setIconSize(QSize(16, 16));
        m_libraryButton->setCheckable(true);
        m_libraryButton->setAutoExclusive(true);
        QObject::connect(m_libraryButton,
                         &QPushButton::clicked,
                         this, &NfMainMenu::libraryClicked);
        menuLayout->addWidget(m_libraryButton);

        auto* foldersShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_1), this);
        foldersShortcut->setContext(Qt::WindowShortcut);
        QObject::connect(foldersShortcut,
                         &QShortcut::activated,
                         this,
                         &NfMainMenu::foldersClicked);

        auto* libraryShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_2), this);
        libraryShortcut->setContext(Qt::WindowShortcut);
        QObject::connect(libraryShortcut,
                         &QShortcut::activated,
                         this,
                         &NfMainMenu::libraryClicked);

        menuLayout->addStretch();

        setLayout(menuLayout);
}

void NfMainMenu::setProjectsMode()
{
        m_projectsButton->setChecked(true);
        m_foldersButton->setChecked(false);
        m_libraryButton->setChecked(false);
}

void NfMainMenu::setFolderMode()
{
        m_projectsButton->setChecked(false);
        m_foldersButton->setChecked(true);
        m_libraryButton->setChecked(false);
}

void NfMainMenu::setLibraryMode()
{
        m_projectsButton->setChecked(false);
        m_foldersButton->setChecked(false);
        m_libraryButton->setChecked(true);
}

} // namespace NfDesktop
