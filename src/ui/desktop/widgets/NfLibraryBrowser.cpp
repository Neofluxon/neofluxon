/**
 * File name: NfLibraryBrowser.cpp
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

#include "NfLibraryBrowser.h"
#include "NfLibraryTreeModel.h"
#include "NfLibraryListView.h"
#include "NfRepresentationListView.h"
#include "NfLibraryTreeView.h"
#include "NfUiState.h"
#include "NfUiLibraryModeState.h"

#include <QVBoxLayout>

using namespace NfUi;

namespace NfDesktop {

NfLibraryBrowser::NfLibraryBrowser(NfLibraryContext ctx,
                                   QWidget *parent)
    : QWidget(parent)
    , m_context{std::move(ctx)}
    , m_state{m_context.uiState()->libraryModeState()}
    , m_model{new NfLibraryTreeModel(m_context, this)}
{
        setupUi();
}

void NfLibraryBrowser::setupUi()
{
        m_mainLayout = new QVBoxLayout(this);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(6);

        m_libraryListView = new NfLibraryListView(m_context, m_model, this);
        m_representationListView = new NfRepresentationListView(m_context, m_model, this);
        m_libraryTreeView = new NfLibraryTreeView(m_context, m_model, this);

        QObject::connect(m_libraryListView->selectionModel(),
                         &QItemSelectionModel::currentChanged,
                         this, [this](const QModelIndex &current, const QModelIndex &) {
                                 m_representationListView->setRootIndex(current);
                                 if (current.isValid()) {
                                         auto firstRep = m_model->index(0, 0, current);
                                         if (firstRep.isValid())
                                                 m_representationListView->setCurrentIndex(firstRep);
                                 }
                         });

        QObject::connect(m_representationListView->selectionModel(),
                         &QItemSelectionModel::currentChanged,
                         this, [this](const QModelIndex &current, const QModelIndex &) {
                                 if (current.isValid())
                                         m_libraryTreeView->setRootIndex(current);
                                 else
                                         m_libraryTreeView->setRootIndex(QModelIndex());
                         });

        auto firstLibrary = m_model->index(0, 0);
        if (firstLibrary.isValid())
                m_libraryListView->setCurrentIndex(firstLibrary);

        m_mainLayout->addWidget(m_libraryListView);
        m_mainLayout->addWidget(m_representationListView);
        m_mainLayout->addWidget(m_libraryTreeView, 1);

        m_mainLayout->setStretch(0, 0);
        m_mainLayout->setStretch(1, 0);
        m_mainLayout->setStretch(2, 1);
}

} // namespace NfDesktop
