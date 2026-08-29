/**
 * File name: NfNewLibraryDialog.h
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

#include "NfNewLibraryDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace NfDesktop {

NfNewLibraryDialog::NfNewLibraryDialog(QWidget* parent)
        : QDialog(parent)
{
        setWindowTitle(tr("New Library"));
        setMinimumWidth(320);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(16, 16, 16, 16);
        mainLayout->setSpacing(8);

        auto* nameLabel = new QLabel(tr("Name"), this);
        m_nameEdit = new QLineEdit(tr("New Library"), this);
        m_nameEdit->selectAll();

        mainLayout->addWidget(nameLabel);
        mainLayout->addWidget(m_nameEdit);
        mainLayout->addStretch();

        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(8);
        auto* cancelButton = new QPushButton(tr("Cancel"), this);
        auto* createButton = new QPushButton(tr("Create"), this);
        createButton->setDefault(true);
        buttonLayout->addStretch();
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(createButton);

        mainLayout->addLayout(buttonLayout);

        QObject::connect(cancelButton,
                         &QPushButton::clicked,
                         this,
                         &QDialog::reject);
        QObject::connect(createButton,
                         &QPushButton::clicked,
                         this,
                         &QDialog::accept);
}

QString NfNewLibraryDialog::name() const
{
        return m_nameEdit->text().trimmed();
}

} // namespace NfDesktop
