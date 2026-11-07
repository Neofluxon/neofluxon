/**
 * File name: NfUserTaskDelegate.cpp
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

#include "NfUserTaskDelegate.h"
#include "NfUserTasksModel.h"
#include "core/NfImportFolderTaskData.h"
#include "core/utils/NfFileUtils.h"

#include <QDateTime>
#include <QPainter>

using namespace NfCore;

namespace NfDesktop {

QSize NfUserTaskDelegate::sizeHint(const QStyleOptionViewItem&,
                                   const QModelIndex&) const
{
        return {360, 116};
}

void NfUserTaskDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
        const auto taskData = index.data(NfUserTasksModel::TaskDataRole);
        if (!taskData.isValid())
                return;

        if (taskData.canConvert<NfImportFolderTaskData>())
                paint(painter, option, taskData.value<NfImportFolderTaskData>());
}

void NfUserTaskDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               NfImportFolderTaskData taskData) const
{
        using Status = NfImportFolderTaskData::Status;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const auto rect = option.rect.adjusted(0, 0, -4, -4);
        const auto textColor = option.palette.color(QPalette::Text);
        auto mutedTextColor = textColor;
        mutedTextColor.setAlphaF(0.68);
        const auto left = rect.left() + 10;
        const auto right = rect.right() - 10;
        const auto status = taskData.status;

        QString statusText;
        switch (status) {
        case Status::Importing:
                statusText = QObject::tr("Importing");
                break;
        case Status::Completed:
                statusText = QObject::tr("Completed");
                break;
        case Status::Failed:
                statusText = QObject::tr("Failed");
                break;
        case Status::Cancelled:
                statusText = QObject::tr("Cancelled");
                break;
        }

        painter->setPen(textColor);
        painter->drawText(QRect(left,
                                rect.top() + 6,
                                rect.width() - 115, 22),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          QObject::tr("Import folder to library"));
        painter->setPen(mutedTextColor);
        painter->drawText(QRect(right - 100,
                                rect.top() + 6,
                                100, 22),
                          Qt::AlignVCenter | Qt::AlignRight, statusText);

        const auto path = QString::fromStdString(
                          NfFileUtils::pathToUtf8(taskData.sourceFolder));
        painter->drawText(QRect(left, rect.top() + 30, rect.width() - 20, 20),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          painter->fontMetrics().elidedText(
                                  path, Qt::ElideMiddle, rect.width() - 20));

        auto descriptionTop = rect.top() + 56;
        if (status == Status::Importing) {
                const QRectF spinner(left, descriptionTop + 1, 14, 14);
                painter->setPen(QPen(option.palette.color(QPalette::Highlight), 2));
                const auto angle = static_cast<int>(
                        (QDateTime::currentMSecsSinceEpoch() / 10) % 360) * 16;
                painter->drawArc(spinner, angle, 250 * 16);
        }

        painter->setPen(mutedTextColor);
        painter->drawText(QRect(left, descriptionTop, rect.width() - 20, 20),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          QObject::tr("Discovered %1  ·  Imported %2  ·  Failed %3")
                          .arg(taskData.discoveredPhotos)
                          .arg(taskData.importedPhotos)
                          .arg(taskData.failedPhotos));

        painter->restore();
}

} // namespace NfDesktop
