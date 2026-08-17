/**
 * File name: NfBrowserViewModeBar.h
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

#include "NfBrowserViewModeBar.h"
#include "NfUiBrowserState.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QFrame>

using namespace NfUi;

namespace NfDesktop {

NfBrowserViewModeBar::NfBrowserViewModeBar(QWidget* parent)
        : NfStyledWidget(parent)
{
        setupUi();
}

void NfBrowserViewModeBar::setState(NfUiBrowserState* state)
{
    if (m_state == state)
        return;

    if (m_state)
            QObject::disconnect(m_state, nullptr, this, nullptr);

    m_state = state;

    if (m_state) {
            auto currentMode = m_state->viewMode();
            if (m_buttons.contains(currentMode))
                    m_buttons[currentMode]->setChecked(true);

            QObject::connect(m_state, &NfUiBrowserState::viewModeChanged,
                             this, [this](NfUiBrowserState::ViewMode newMode) {
                                     if (m_buttons.contains(newMode)) {
                                             QSignalBlocker blocker(m_buttonGroup);
                                             m_buttons[newMode]->setChecked(true);
                                     }
                             });
    }
}

NfUiBrowserState* NfBrowserViewModeBar::getState() const
{
        return m_state;
}

void NfBrowserViewModeBar::setupUi()
{
        auto* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        m_buttonGroup = new QButtonGroup(this);
        m_buttonGroup->setExclusive(true);

        QObject::connect(m_buttonGroup, &QButtonGroup::idClicked, this, [this](int id) {
                if (m_state)
                        m_state->setViewMode(static_cast<ViewMode>(id));
        });

        createModeButton(ViewMode::Grid, tr("Grid"), QIcon(":/icons/viewmode_grid.svg"));
        createModeButton(ViewMode::Preview, tr("Preview"), QIcon(":/icons/viewmode_preview.svg"));

        for (auto* button : m_buttons)
                mainLayout->addWidget(button);
}

void NfBrowserViewModeBar::createModeButton(ViewMode mode,
                                            const QString& text,
                                            const QIcon& icon)
{
        auto* button = new QToolButton(this);
        button->setText(text);
        button->setIcon(icon);
        button->setCheckable(true);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setCursor(Qt::PointingHandCursor);

        m_buttonGroup->addButton(button, static_cast<int>(mode));
        m_buttons.insert(mode, button);
}

} // namespace NfDesktop
