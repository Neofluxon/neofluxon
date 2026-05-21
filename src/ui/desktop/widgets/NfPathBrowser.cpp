/**
 * File name: NfPathBrowser.h
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

#include "NfPathBrowser.h"
#include "NfFolderActionDelegate.h"
#include "core/NfLogger.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QFileSystemModel>
#include <QDir>

namespace NfDesktop {

NfPathBrowser::NfPathBrowser(QWidget* parent)
        : QWidget(parent)
{
        auto layout = new QVBoxLayout(this);

        // Title
        auto title = new QLabel(tr("Folders"), this);
        QFont f = title->font();
        f.setBold(true);
        title->setFont(f);

        layout->addWidget(title);

        // File system model
        m_model = new QFileSystemModel(this);
        m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        m_model->setRootPath(QDir::homePath()); // or "/" for full system

        // Tree view
        m_treeView = new QTreeView(this);
        m_treeView->setHeaderHidden(true);
        m_treeView->setRootIsDecorated(true);
        m_treeView->setModel(m_model);
        m_treeView->viewport()->setAttribute(Qt::WA_Hover);
        m_treeView->viewport()->setMouseTracking(true);

        // Optional: hide unnecessary columns
        m_treeView->setColumnHidden(1, true); // Size
        m_treeView->setColumnHidden(2, true); // Type
        m_treeView->setColumnHidden(3, true); // Date modified

        // Set root index
        m_treeView->setRootIndex(m_model->index(QDir::rootPath()));

        // Behavior
        m_treeView->setHeaderHidden(true);
        m_treeView->setAnimated(true);
        m_treeView->setIndentation(16);

        layout->addWidget(m_treeView);

        layout->setContentsMargins(0, 0, 0, 0);

        auto folderDelegate = new NfFolderActionDelegate(this);
        m_treeView->setItemDelegate(folderDelegate);

        QObject::connect(folderDelegate,
                         &NfFolderActionDelegate::importRequested,
                         this, [this](const QString& folderPath) {
                                 NF_LOG_DEBUG("import folder: " << folderPath.toStdString());
                         });

        QObject::connect(m_treeView, &QTreeView::doubleClicked,
                         this, [this](const QModelIndex& index)
                         {
                                 emit folderSelected(m_model->filePath(index).toStdString());
                         });
}

} // namespace NfDesktop
