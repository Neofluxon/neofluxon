/**
 * File name: NfBrowserSortBar.cpp
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

#include "NfBrowserSortBar.h"
#include "NfUiBrowserState.h"
#include "core/NfLogger.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QSignalBlocker>
#include <QToolButton>
#include <QActionGroup>

using namespace NfUi;

namespace NfDesktop {

NfBrowserSortBar::NfBrowserSortBar(NfUiBrowserState *state, QWidget* parent)
        : NfStyledWidget(parent)
        , m_state{state}
{
        setObjectName("NfBrowserSortBar");
        QObject::connect(m_state,
                         &NfUiBrowserState::sortingUpdated,
                         this,
                         &NfBrowserSortBar::updateSorting);
        setupUi();
        updateSorting();
}

void NfBrowserSortBar::setupUi()
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        m_button = new QToolButton(this);
        m_button->setPopupMode(QToolButton::InstantPopup);

        auto* menu = new QMenu(m_button);

        const auto addFiledAction = [this, menu] (const QString& label,
                                                  SortField field) -> QAction* {
                auto* action = menu->addAction(label);
                action->setCheckable(true);
                QObject::connect(action,
                                 &QAction::triggered,
                                 [this, field](bool b) {
                                         m_state->enableSortField(field, b);
                                         NF_LOG_DEBUG("sortfileds: " << m_state->sortingFields().size());
                                 });
                m_fieldActions.insert({field, action});

                return action;
        };

        auto* fieldGroup = new QActionGroup(menu);
        fieldGroup->addAction(addFiledAction(tr("Name"), SortField::Name));
        fieldGroup->addAction(addFiledAction(tr("Date Taken"), SortField::DateTaken));
        fieldGroup->addAction(addFiledAction(tr("File Size"), SortField::FileSize));

        menu->addSeparator();

        const auto addOrderAction = [this, menu] (const QString& label,
                                                  SortOrder sortOrder) -> QAction* {
                auto* action = menu->addAction(label);
                action->setCheckable(true);
                QObject::connect(action,
                                 &QAction::triggered,
                                 [this, sortOrder](bool b) {
                                         m_state->setSortOrder(sortOrder);
                                 });

                return action;
        };

        auto* orderGroup = new QActionGroup(menu);
        orderGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
        m_ascendingAction = addOrderAction(tr("Ascending"), SortOrder::Ascending);
        orderGroup->addAction(m_ascendingAction);
        m_descendingAction = addOrderAction(tr("Descending"), SortOrder::Descending);
        orderGroup->addAction(m_descendingAction);

        m_button->setMenu(menu);

        layout->addWidget(m_button);
}

void NfBrowserSortBar::updateSorting()
{
        for (const auto &act : m_fieldActions)
                act.second->setChecked(false);

        int nFields = 0;
        for (const auto &field : m_state->sortingFields()) {
                if (auto res = m_fieldActions.find(field);
                    res != m_fieldActions.end()) {
                        const QSignalBlocker blocker(res->second);
                        res->second->setChecked(true);
                        nFields++;
                }
        }

        auto sortOrder = m_state->sortOrder();
        {
                const QSignalBlocker blocker(m_ascendingAction);
                m_ascendingAction->setChecked(sortOrder == SortOrder::Ascending);
                m_ascendingAction->setEnabled(nFields > 0);
        }
        {
                const QSignalBlocker blocker(m_descendingAction);
                m_descendingAction->setChecked(sortOrder == SortOrder::Descending);
                m_descendingAction->setEnabled(nFields > 0);
        }

        auto orderText = sortOrder ==  SortOrder::Ascending ? tr("Asc") : tr("Des");
        auto text = QString("Sort (%1) %2").arg(nFields)
                .arg(nFields > 0 ? orderText : "" );
        m_button->setText(text);
}

} // namespace NfDesktop
