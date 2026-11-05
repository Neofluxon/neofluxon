/**
 * File name: NfBagroundTaskDelegate.cpp
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

#include "NfBagroundTaskDelegate.h"
#include "NfBagroundTasksModel.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

namespace NfDesktop {

QSize NfBagroundTaskDelegate::sizeHint(const QStyleOptionViewItem&,
                                       const QModelIndex&) const
{
        return {360, 104};
}

bool NfBagroundTaskDelegate::editorEvent(QEvent* event,
                                         QAbstractItemModel* model,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index)
{
        if (event->type() != QEvent::MouseButtonRelease)
                return false;

        const auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() != Qt::LeftButton)
                return false;

        const auto rect = option.rect.adjusted(0, 0, -4, -4);
        const auto actionRect = QRect(rect.right() - 2 - 76,
                                      rect.top() + 57,
                                      76,
                                      25);
        if (!actionRect.contains(mouseEvent->pos()))
                return false;

        if (auto* taskModel = qobject_cast<NfBagroundTasksModel*>(model))
                taskModel->requestAction(index);
        return true;
}

void NfBagroundTaskDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const auto rect = option.rect.adjusted(0, 0, -4, -4);
        const auto state = static_cast<NfUi::NfBackgroundTask::State>(
                index.data(NfBagroundTasksModel::StateRole).toInt());
        const auto stateText = state == NfUi::NfBackgroundTask::State::Running
                ? QObject::tr("Running")
                : state == NfUi::NfBackgroundTask::State::Paused
                        ? QObject::tr("Paused")
                        : QObject::tr("Stopped");
        const auto textColor = option.palette.color(QPalette::Text);
        auto mutedTextColor = textColor;
        mutedTextColor.setAlphaF(0.68);
        const auto left = rect.left() + 2;
        const auto right = rect.right() - 2;

        painter->setPen(textColor);
        painter->drawText(QRect(left, rect.top(), rect.width() - 90, 20),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          index.data(NfBagroundTasksModel::OperationRole).toString());
        painter->setPen(mutedTextColor);
        painter->drawText(QRect(right - 86, rect.top(), 86, 20),
                          Qt::AlignVCenter | Qt::AlignRight,
                          stateText);

        painter->setPen(mutedTextColor);
        const auto path = index.data(NfBagroundTasksModel::SourcePathRole).toString();
        painter->drawText(QRect(left, rect.top() + 22, rect.width() - 8, 18),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          painter->fontMetrics().elidedText(
                                  path, Qt::ElideMiddle, rect.width() - 8));

        const QRect progressRect(left, rect.top() + 46, rect.width() - 8, 6);
        painter->setPen(Qt::NoPen);
        painter->setBrush(option.palette.color(QPalette::AlternateBase));
        painter->drawRoundedRect(progressRect, 3, 3);
        auto progressRectFill = progressRect;
        progressRectFill.setWidth(progressRect.width()
                                  * index.data(NfBagroundTasksModel::ProgressRole)
                                            .toInt() / 100);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRoundedRect(progressRectFill, 3, 3);

        painter->setPen(mutedTextColor);
        painter->drawText(QRect(left, rect.top() + 58, 210, 24),
                          Qt::AlignVCenter | Qt::AlignLeft,
                          index.data(NfBagroundTasksModel::ProgressTextRole)
                                  .toString());

        const auto action = index.data(NfBagroundTasksModel::ActionTextRole).toString();
        const auto actionRect = QRect(right - 76, rect.top() + 57, 76, 25);
        painter->setPen(mutedTextColor);
        painter->setBrush(Qt::transparent);
        painter->drawRoundedRect(actionRect, 3, 3);
        painter->setPen(textColor);
        painter->drawText(actionRect, Qt::AlignCenter, action);

        painter->restore();
}

} // namespace NfDesktop
