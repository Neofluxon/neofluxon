/**
 * File name: NfStorageListView.h
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

#include "NfStorageListView.h"

#include <QDir>
#include <QFrame>
#include <QIcon>
#include <QListWidget>
#include <QSet>
#include <QStorageInfo>
#include <QVBoxLayout>

namespace NfDesktop {

namespace {
constexpr int itemHeight = 28;
constexpr int maxVisibleItems = 8;
}

NfStorageListView::NfStorageListView(QWidget* parent)
        : QWidget(parent)
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* storage = new QListWidget(this);
        storage->setFrameShape(QFrame::NoFrame);
        storage->setIconSize(QSize(14, 14));
        QSet<QString> addedPaths;
        for (const auto& volume : QStorageInfo::mountedVolumes()) {
                if (!volume.isValid() || !volume.isReady())
                        continue;

                const auto path = QDir::cleanPath(volume.rootPath());
                if (path.isEmpty() || addedPaths.contains(path))
                        continue;

#if defined(Q_OS_LINUX)
                if (path != QDir::rootPath()
                    && !volume.device().startsWith(QByteArrayLiteral("/dev/")))
                        continue;
#endif

                auto label = volume.displayName();
                if (label.isEmpty())
                        label = volume.device();
                if (label.isEmpty())
                        label = path;

                auto* item = new QListWidgetItem(label, storage);
                item->setIcon(QIcon(QStringLiteral(":/icons/place-storage.svg")));
                item->setSizeHint(QSize(0, itemHeight));
                item->setData(Qt::UserRole, path);
                item->setToolTip(path);
                addedPaths.insert(path);
        }
        QObject::connect(storage, &QListWidget::itemClicked,
                         this, [this](QListWidgetItem* item) {
                                 emit pathSelected(item->data(Qt::UserRole).toString());
                         });

        int maxHeight = maxVisibleItems * itemHeight
                + 2 * storage->frameWidth();
        storage->setMaximumHeight(maxHeight);
        setMaximumHeight(maxHeight);
        layout->addWidget(storage);

}

} // namespace NfDesktop
