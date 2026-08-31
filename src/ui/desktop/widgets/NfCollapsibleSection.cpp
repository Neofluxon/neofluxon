/**
 * File name: NfCollapsibleSection.cpp
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

#include "NfCollapsibleSection.h"

#include <QToolButton>
#include <QFrame>
#include <QVBoxLayout>

namespace NfDesktop {

NfCollapsibleSection::NfCollapsibleSection(const QString& title, QWidget* parent)
        : QWidget(parent)
{
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(0);

        m_toggleButton = new QToolButton(this);
        m_toggleButton->setCheckable(true);
        m_toggleButton->setText(title);
        m_toggleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        // By default collapsed
        m_toggleButton->setChecked(false);
        m_toggleButton->setArrowType(Qt::RightArrow);

        m_contentFrame = new QFrame(this);
        m_mainLayout->addWidget(m_toggleButton);
        m_mainLayout->addWidget(m_contentFrame);
        m_contentFrame->setVisible(false);

        QObject::connect(m_toggleButton,
                         &QToolButton::toggled,
                         this,
                         &NfCollapsibleSection::toggle);
}

void NfCollapsibleSection::setContent(QWidget* widget)
{
        if (m_contentFrame->layout())
                delete m_contentFrame->layout();

        if (!widget)
                return;

        auto* layout = new QVBoxLayout(m_contentFrame);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(widget);
}

void NfCollapsibleSection::toggle(bool checked)
{
        m_toggleButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
        m_contentFrame->setVisible(checked);
}

} // namespace NfDesktop
