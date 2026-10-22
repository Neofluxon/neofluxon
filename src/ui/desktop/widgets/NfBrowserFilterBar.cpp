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

#include <QAction>
#include <QSignalBlocker>
#include <QHBoxLayout>
#include <QMenu>

namespace NfDesktop {

NfBrowserFilterBar::NfBrowserFilterBar(NfUi::NfUiBrowserState* state,
                                       QWidget* parent)
        : NfStyledWidget(parent)
        , m_state{state}
{
        setupUi();
        QObject::connect(m_state,
                         &NfUi::NfUiBrowserState::formatFilterChanged,
                         this,
                         &NfBrowserFilterBar::updateFormatFilter);
        updateFormatFilter();
}

void NfBrowserFilterBar::setupUi()
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_button = new QToolButton(this);
        m_button->setPopupMode(QToolButton::InstantPopup);

        auto* menu = new QMenu(m_button);

        const auto addFormatAction = [this, menu](const QString& label,
                                                  NfCore::NfPhoto::PhotoFormat format) {
                auto* action = menu->addAction(label);
                action->setCheckable(true);
                m_formatActions.emplace_back(format, action);
                QObject::connect(action, &QAction::toggled, this,
                                 [this, format](bool checked) {
                                         auto formats = m_state->formatFilter();
                                         if (checked)
                                                 formats.insert(format);
                                         else
                                                 formats.erase(format);
                                         m_state->setFormatFilter(formats);
                                 });
        };

        addFormatAction(tr("RAW"), NfCore::NfPhoto::PhotoFormat::Raw);
        addFormatAction(tr("JPEG"), NfCore::NfPhoto::PhotoFormat::Jpeg);
        addFormatAction(tr("PNG"), NfCore::NfPhoto::PhotoFormat::Png);

        menu->addSeparator();
        m_clearAction = menu->addAction(tr("Clear format filters"));
        QObject::connect(m_clearAction, &QAction::triggered,
                         m_state, [this]() {
                                 m_state->setFormatFilter({});
                         });

        m_button->setMenu(menu);

        layout->addWidget(m_button);
}

void NfBrowserFilterBar::updateFormatFilter()
{
        const auto& formats = m_state->formatFilter();
        for (const auto& [format, action] : m_formatActions) {
                const QSignalBlocker blocker(action);
                action->setChecked(formats.contains(format));
        }

        m_clearAction->setEnabled(!formats.empty());
        m_button->setText(tr("Filter (%1)").arg(formats.size()));
}

} // namespace NfDesktop
