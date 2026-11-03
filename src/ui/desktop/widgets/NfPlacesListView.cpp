/**
 * File name: NfPlacesListView.h
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

#include "NfPlacesListView.h"

#include <QListWidget>
#include <QDir>
#include <QFrame>
#include <QFileInfo>
#include <QIcon>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QtGlobal>

namespace NfDesktop {

namespace {
constexpr int itemHeight = 28;
constexpr int maxVisibleItems = 5;
}

NfPlacesListView::NfPlacesListView(QWidget* parent)
        : QWidget(parent)
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* places = new QListWidget(this);
        places->setFrameShape(QFrame::NoFrame);
        places->setIconSize(QSize(14, 14));
        QStringList addedPaths;

#if defined(Q_OS_WIN)
        addPlace(places, tr("User"),
                 QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                 QStringLiteral(":/icons/place-home.svg"),
                 addedPaths);
        addPlace(places, tr("Desktop"),
                 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                 QStringLiteral(":/icons/place-desktop.svg"),
                 addedPaths);
        addPlace(places, tr("Downloads"),
                 QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
                 QStringLiteral(":/icons/place-downloads.svg"),
                 addedPaths);
        addPlace(places, tr("Pictures"),
                 QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                 QStringLiteral(":/icons/place-pictures.svg"),
                 addedPaths);
        addPlace(places, tr("Documents"),
                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                 QStringLiteral(":/icons/place-documents.svg"),
                 addedPaths);
#elif defined(Q_OS_LINUX)
        addPlace(places, tr("/"), QDir::rootPath(),
                 QStringLiteral(":/icons/place-root.svg"), addedPaths);
        addPlace(places, tr("Home"),
                 QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                 QStringLiteral(":/icons/place-home.svg"),
                 addedPaths);
        addPlace(places, tr("Desktop"),
                 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                 QStringLiteral(":/icons/place-desktop.svg"),
                 addedPaths);
        addPlace(places, tr("Downloads"),
                 QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
                 QStringLiteral(":/icons/place-downloads.svg"),
                 addedPaths);
        addPlace(places, tr("Pictures"),
                 QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                 QStringLiteral(":/icons/place-pictures.svg"),
                 addedPaths);
#endif
        QObject::connect(places, &QListWidget::itemClicked,
                         this, [this](QListWidgetItem* item) {
                                 emit pathSelected(item->data(Qt::UserRole).toString());
                         });

        const auto maxHeight = maxVisibleItems * itemHeight
                + 2 * places->frameWidth();
        places->setMaximumHeight(maxHeight);
        setMaximumHeight(maxHeight);

        layout->addWidget(places);
}

void NfPlacesListView::addPlace(QListWidget* places,
                                const QString& label,
                                const QString& path,
                                const QString& iconPath,
                                QStringList& addedPaths)
{
        if (path.isEmpty() || addedPaths.contains(path))
                return;

        const QFileInfo info(path);
        if (!info.exists() || !info.isDir())
                return;

        const auto absolutePath = info.absoluteFilePath();
        if (addedPaths.contains(absolutePath))
                return;

        auto* item = new QListWidgetItem(label, places);
        item->setIcon(QIcon(iconPath));
        item->setSizeHint(QSize(0, itemHeight));
        item->setData(Qt::UserRole, absolutePath);
        addedPaths.append(absolutePath);
}

} // namespace NfDesktop
