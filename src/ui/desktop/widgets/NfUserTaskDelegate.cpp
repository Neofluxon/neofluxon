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
 */

#include "NfUserTaskDelegate.h"

#include "NfUserTasksModel.h"

#include <QDateTime>
#include <QPainter>

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
        const auto taskType = static_cast<NfCore::NfUserTaskType>(
                index.data(NfUserTasksModel::TaskTypeRole).toInt());
        switch (taskType) {
        case NfCore::NfUserTaskType::ImportFolder:
                break;
        default:
                return;
        }

        using Status = NfCore::NfImportFolderTaskData::Status;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const auto rect = option.rect.adjusted(0, 0, -4, -4);
        const auto textColor = option.palette.color(QPalette::Text);
        auto mutedTextColor = textColor;
        mutedTextColor.setAlphaF(0.68);
        const auto left = rect.left() + 10;
        const auto right = rect.right() - 10;
        const auto status = static_cast<Status>(
                index.data(NfUserTasksModel::StatusRole).toInt());

        QString statusText;
        switch (status) {
        case Status::Scanning:
                statusText = QObject::tr("Scanning");
                break;
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
        painter->drawText(QRect(left, rect.top() + 6, rect.width() - 115, 22),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          index.data(Qt::DisplayRole).toString());
        painter->setPen(mutedTextColor);
        painter->drawText(QRect(right - 100, rect.top() + 6, 100, 22),
                          Qt::AlignVCenter | Qt::AlignRight, statusText);

        const auto path = index.data(NfUserTasksModel::SourcePathRole).toString();
        painter->drawText(QRect(left, rect.top() + 30, rect.width() - 20, 20),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          painter->fontMetrics().elidedText(
                                  path, Qt::ElideMiddle, rect.width() - 20));

        auto descriptionTop = rect.top() + 56;
        if (status == Status::Scanning) {
                const QRectF spinner(left, descriptionTop + 1, 14, 14);
                painter->setPen(QPen(option.palette.color(QPalette::Highlight), 2));
                const auto angle = static_cast<int>(
                        (QDateTime::currentMSecsSinceEpoch() / 10) % 360) * 16;
                painter->drawArc(spinner, angle, 250 * 16);
                painter->setPen(mutedTextColor);
                painter->drawText(QRect(left + 22, descriptionTop - 1,
                                        rect.width() - 42, 18),
                                  Qt::AlignVCenter | Qt::AlignLeft,
                                  QObject::tr("Scanning folder structure..."));
        } else if (status == Status::Importing) {
                const QRectF spinner(left, descriptionTop + 1, 14, 14);
                painter->setPen(QPen(option.palette.color(QPalette::Highlight), 2));
                const auto angle = static_cast<int>(
                        (QDateTime::currentMSecsSinceEpoch() / 10) % 360) * 16;
                painter->drawArc(spinner, angle, 250 * 16);
                painter->setPen(mutedTextColor);
                painter->drawText(QRect(left + 22, descriptionTop - 1,
                                        rect.width() - 42, 18),
                                  Qt::AlignVCenter | Qt::AlignLeft,
                                  QObject::tr("Processed %1 photos")
                                          .arg(index.data(
                                                  NfUserTasksModel::ProcessedPhotosRole)
                                                       .toULongLong()));
                descriptionTop += 22;
                painter->drawText(
                        QRect(left, descriptionTop, rect.width() - 20, 18),
                        Qt::AlignVCenter | Qt::AlignLeft,
                        QObject::tr("Imported %1  ·  Skipped %2  ·  Failed %3")
                                .arg(index.data(NfUserTasksModel::ImportedPhotosRole)
                                             .toULongLong())
                                .arg(index.data(NfUserTasksModel::SkippedPhotosRole)
                                             .toULongLong())
                                .arg(index.data(NfUserTasksModel::FailedPhotosRole)
                                             .toULongLong()));
        } else {
                painter->setPen(mutedTextColor);
                painter->drawText(
                        QRect(left, descriptionTop, rect.width() - 20, 20),
                        Qt::AlignVCenter | Qt::AlignLeft,
                        QObject::tr("Imported %1  ·  Skipped %2  ·  Failed %3")
                                .arg(index.data(NfUserTasksModel::ImportedPhotosRole)
                                             .toULongLong())
                                .arg(index.data(NfUserTasksModel::SkippedPhotosRole)
                                             .toULongLong())
                                .arg(index.data(NfUserTasksModel::FailedPhotosRole)
                                             .toULongLong()));
        }

        painter->restore();
}

} // namespace NfDesktop
