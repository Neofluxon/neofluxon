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
#include "NfLibraryListHeader.h"
#include "NfNewLibraryDialog.h"
#include "NfLibraryListView.h"
#include "NfRepresentationListView.h"
#include "NfLibraryTreeView.h"
#include "NfUiState.h"
#include "NfUiLibraryModeState.h"
#include "NfLibraryAdapter.h"
#include "core/NfLogger.h"

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

        auto libraryListHeader = new NfLibraryListHeader(this);
        QObject::connect(libraryListHeader,
                         &NfLibraryListHeader::addLibrary,
                         this,
                         &NfLibraryBrowser::addLibrary);

        m_libraryListView = new NfLibraryListView(m_context, m_model, this);
        m_representationListView = new NfRepresentationListView(m_context, m_model, this);
        m_libraryTreeView = new NfLibraryTreeView(m_context, m_model, this);

        QObject::connect(m_libraryListView->selectionModel(),
                         &QItemSelectionModel::currentChanged,
                         this, [this](const QModelIndex &current, const QModelIndex &) {
                                 // Reset the representation view when no library is selected.
                                 if (!current.isValid()) {
                                         m_representationListView->setRootIndex(QModelIndex());
                                         return;
                                 }

                                 // Set the library as the root so its child representations are shown.
                                 m_representationListView->setRootIndex(current);

                                 auto role = NfLibraryTreeModel::LibraryIdRole;
                                 auto libraryId = m_model->data(current, role).toULongLong();
                                 m_state->setCurrentLibrary(libraryId);

                                 // Select the first representation, if one exists.
                                 auto firstRep = m_model->index(0, 0, current);
                                 if (firstRep.isValid())
                                         m_representationListView->setCurrentIndex(firstRep);
                         });

        QObject::connect(m_representationListView->selectionModel(),
                         &QItemSelectionModel::currentChanged,
                         this, [this](const QModelIndex &current, const QModelIndex &) {
                                 if (current.isValid())
                                         m_libraryTreeView->setRootIndex(current);
                                 else
                                         m_libraryTreeView->setRootIndex(QModelIndex());
                         });

        // Find the library's model index using the current library ID from state.
        QModelIndex selectedLibrary;
        const auto currentLibraryId = m_state->currentLibrary();
        for (int row = 0; row < m_model->rowCount(); ++row) {
                const auto index = m_model->index(row, 0);
                const auto role = NfLibraryTreeModel::LibraryIdRole;
                if (m_model->data(index, role).toULongLong() == currentLibraryId) {
                        selectedLibrary = index;
                        break;
                }
        }

        // Select the first library if the current library ID was not found.
        if (!selectedLibrary.isValid() && m_model->rowCount() > 0)
                selectedLibrary = m_model->index(0, 0);

        // Select the current library in the list view.
        if (selectedLibrary.isValid())
                m_libraryListView->setCurrentIndex(selectedLibrary);

        m_mainLayout->addWidget(libraryListHeader);
        m_mainLayout->addWidget(m_libraryListView);
        m_mainLayout->addWidget(m_representationListView);
        m_mainLayout->addWidget(m_libraryTreeView, 1);

        m_mainLayout->setStretch(0, 0);
        m_mainLayout->setStretch(1, 0);
        m_mainLayout->setStretch(2, 1);
}

void NfLibraryBrowser::addLibrary()
{
        NfNewLibraryDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
                auto name = dialog.name();
                if (!name.isEmpty())
                        m_context.library()->addLibrary(name.toStdString());
        }
}

} // namespace NfDesktop
