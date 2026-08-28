/**
 * File name: NfLibraryListHeader.cpp
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

#include "NfLibraryListHeader.h"

#include <QHBoxLayout>

namespace NfDesktop {

NfLibraryListHeader::NfLibraryListHeader(QWidget* parent)
        : QWidget(parent)
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 4, 8, 4);

        auto* titleLabel = new QLabel(tr("LIBRARIES"), this);

        m_addButton = new QPushButton("+", this);
        m_addButton->setFixedSize(20, 20);
        m_addButton->setVisible(false);

        layout->addWidget(titleLabel);
        layout->addStretch();
        layout->addWidget(m_addButton);

        connect(m_addButton,
                &QPushButton::clicked,
                this,
                &NfLibraryListHeader::addLibrary);
}

void NfLibraryListHeader::enterEvent(QEnterEvent* event)
{
        m_addButton->setVisible(true);
        QWidget::enterEvent(event);
}

void NfLibraryListHeader::leaveEvent(QEvent* event)
{
        m_addButton->setVisible(false);
        QWidget::leaveEvent(event);
}

} // namespace NfDesktop
