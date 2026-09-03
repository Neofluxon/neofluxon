/**
 * File name: NfMetadataSectionWidget.cpp
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

#include "NfMetadataSectionWidget.h"
#include "NfImageMetadataModel.h"
#include "NfMetadataFilterProxy.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

namespace NfDesktop {

NfMetadataSectionWidget::NfMetadataSectionWidget(const QSet<QString>& allowedKeys, QWidget* parent)
        : QWidget(parent), m_allowedKeys(allowedKeys)
{
        setupUi();
}

void NfMetadataSectionWidget::setModel(NfImageMetadataModel* model)
{
        if (!masterModel) {
                m_tableView->setModel(nullptr);
                return;
        }

        auto* proxy = new NfMetadataFilterProxy(m_allowedKeys, this);
        proxy->setSourceModel(masterModel);
        m_tableView->setModel(proxy);
}

void NfMetadataSectionWidget::setupUi()
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        m_tableView = new QTableView(this);
        m_tableView->horizontalHeader()->setVisible(false);
        m_tableView->verticalHeader()->setVisible(false);
        m_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_tableView->setShowGrid(false);
        m_tableView->setSelectionMode(QAbstractItemView::NoSelection);
        m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        layout->addWidget(m_tableView);
}

} // namespace NfDesktop
