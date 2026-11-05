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
#include <QIcon>
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

        m_sortButton = new QToolButton(this);
        m_sortButton->setObjectName("sortOrderButton");
        m_sortButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_sortButton->setIconSize(QSize(18, 18));
        m_sortButton->setFixedSize(32, 32);
        m_sortButton->setAutoRaise(true);
        QObject::connect(m_sortButton, &QToolButton::clicked, this, [this]() {
                m_state->sortOrder() == SortOrder::Ascending
                        ? m_state->setSortOrder(SortOrder::Descending)
                        : m_state->setSortOrder(SortOrder::Ascending);
        });
        layout->addWidget(m_sortButton);

        m_fieldButton = new QToolButton(this);
        m_fieldButton->setObjectName("sortFieldButton");
        m_fieldButton->setPopupMode(QToolButton::InstantPopup);
        m_fieldButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
        m_fieldButton->setMinimumHeight(32);
        m_fieldButton->setToolTip(tr("Sort field"));

        auto* menu = new QMenu(this);
        const auto addFiledAction = [this, menu] (const QString& label,
                                                  SortField field) -> QAction* {
                auto* action = menu->addAction(label);
                action->setCheckable(true);
                QObject::connect(action,
                                 &QAction::triggered,
                                 [this, field](bool b) {
                                         {
                                                 const QSignalBlocker blocker(m_state);
                                                 m_state->setSortFields({});
                                         }
                                         m_state->enableSortField(field, b);
                                         NF_LOG_DEBUG("sortfileds: " << m_state->sortingFields().size());
                                 });
                m_fieldActions.insert({field, action});

                return action;
        };

        auto* fieldGroup = new QActionGroup(menu);
        fieldGroup->addAction(addFiledAction(tr("Name"),
                                             SortField::Name));
        fieldGroup->addAction(addFiledAction(tr("Date Taken"),
                                             SortField::DateTaken));
        fieldGroup->addAction(addFiledAction(tr("File Size"),
                                             SortField::FileSize));

        m_fieldButton->setMenu(menu);

        layout->addWidget(m_fieldButton);
}

void NfBrowserSortBar::updateSorting()
{
        for (const auto &act : m_fieldActions)
                act.second->setChecked(false);

        QString fieldText = tr("Default");
        for (const auto &field : m_state->sortingFields()) {
                if (auto res = m_fieldActions.find(field);
                    res != m_fieldActions.end()) {
                        const QSignalBlocker blocker(res->second);
                        res->second->setChecked(true);
                        fieldText = res->second->iconText();
                        break;
                }
        }
        m_fieldButton->setText(fieldText);

        m_sortButton->setIcon(QIcon(m_state->sortOrder() == SortOrder::Ascending
                                    ? ":/icons/sort-ascending.svg"
                                    : ":/icons/sort-descending.svg"));
        m_sortButton->setToolTip(m_state->sortOrder() == SortOrder::Ascending
                                 ? tr("Sort descending")
                                 : tr("Sort ascending"));
}

} // namespace NfDesktop
