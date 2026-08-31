/**
 * File name: NfScrollStack.cpp
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

#include "NfScrollStack.h"

#include <QVBoxLayout>
#include <QWidget>

namespace NfDesktop {

NfScrollStack::NfScrollStack(QWidget* parent)
        : QScrollArea(parent)
{
        setWidgetResizable(true);
        setFrameShape(QFrame::NoFrame);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        m_scrollContent = new QWidget(this);
        m_mainLayout = new QVBoxLayout(m_scrollContent);
        m_mainLayout->setContentsMargins(4, 4, 4, 4);
        m_mainLayout->setSpacing(2);
        m_mainLayout->setAlignment(Qt::AlignTop);

        setWidget(m_scrollContent);
}

void NfScrollStack::addWidget(QWidget* widget)
{
        m_mainLayout->addWidget(widget);
}

void NfScrollStack::insertWidget(int index, QWidget* widget)
{
        m_mainLayout->insertWidget(index, widget);
}

void NfScrollStack::addStretch(int stretch)
{
        m_mainLayout->addStretch(stretch);
}

} // namespace NfDesktop
