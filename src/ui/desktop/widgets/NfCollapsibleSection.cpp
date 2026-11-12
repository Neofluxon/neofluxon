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

#include <QIcon>
#include <QToolButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>

namespace NfDesktop {

NfCollapsibleSection::NfCollapsibleSection(const QString& title,
                                           bool collapsible,
                                           QWidget* parent)
        : QWidget(parent)
        , m_collapsible{collapsible}
{
        setAttribute(Qt::WA_StyledBackground);
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(0);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        if (m_collapsible) {
                m_toggleButton = new QToolButton(this);
                m_toggleButton->setCheckable(true);
                m_toggleButton->setText(title);
                m_toggleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                m_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                m_toggleButton->setIconSize(QSize(12, 12));
                m_toggleButton->setIcon(QIcon(QStringLiteral(":/icons/chevron-right.svg")));
                m_toggleButton->setChecked(false);
                m_mainLayout->addWidget(m_toggleButton);
                QObject::connect(m_toggleButton,
                                 &QToolButton::toggled,
                                 this,
                                 &NfCollapsibleSection::toggle);
        } else {
                m_titleLabel = new QLabel(title, this);
                m_titleLabel->setObjectName(QStringLiteral("SectionTitle"));
                m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                m_titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                m_mainLayout->addWidget(m_titleLabel);
        }

        m_contentFrame = new QFrame(this);
        m_contentFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_mainLayout->addWidget(m_contentFrame);
        m_contentFrame->setVisible(!m_collapsible);
}

void NfCollapsibleSection::setContent(QWidget* widget)
{
        if (m_contentFrame->layout())
                delete m_contentFrame->layout();

        if (!widget)
                return;

        const auto expandsVertically =
                widget->sizePolicy().verticalPolicy() == QSizePolicy::Expanding;
        if (expandsVertically) {
                m_contentFrame->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Expanding);
        } else {
                m_contentFrame->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);
                m_contentFrame->setMaximumHeight(widget->maximumHeight());
        }

        auto* layout = new QVBoxLayout(m_contentFrame);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(widget);
}

bool NfCollapsibleSection::isExpanded() const
{
        return m_collapsible && m_toggleButton->isChecked();
}

void NfCollapsibleSection::setExpanded(bool expanded)
{
        if (!m_collapsible)
                return;

        m_toggleButton->setChecked(expanded);
}

void NfCollapsibleSection::toggle(bool checked)
{
        if (!m_collapsible)
                return;

        m_toggleButton->setIcon(QIcon(checked
                                      ? QStringLiteral(":/icons/chevron-down.svg")
                                      : QStringLiteral(":/icons/chevron-right.svg")));
        m_contentFrame->setVisible(checked);
        emit expandedChanged(checked);
}

} // namespace NfDesktop
