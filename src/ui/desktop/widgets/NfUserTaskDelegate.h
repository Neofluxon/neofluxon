/**
 * File name: NfUserTaskDelegate.h
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

#ifndef NF_USER_TASK_DELEGATE_H
#define NF_USER_TASK_DELEGATE_H

#include <QStyledItemDelegate>

namespace NfDesktop {

class NfUserTaskDelegate : public QStyledItemDelegate
{
public:
        using QStyledItemDelegate::QStyledItemDelegate;

        QSize sizeHint(const QStyleOptionViewItem& option,
                       const QModelIndex& index) const override;
        void paint(QPainter* painter,
                   const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

} // namespace NfDesktop

#endif // NF_USER_TASK_DELEGATE_H
