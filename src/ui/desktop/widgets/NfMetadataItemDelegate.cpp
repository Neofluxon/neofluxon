/**
 * File name: NfMetadataItemDelegate.cpp
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

#include "NfMetadataItemDelegate.h"

#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QWidget>

namespace NfDesktop {

NfMetadataItemDelegate::NfMetadataItemDelegate(QWidget* styleSource,
                                               QObject* parent)
        : QStyledItemDelegate(parent)
        , m_styleSource{styleSource}
{
}

void NfMetadataItemDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
        QStyleOptionViewItem itemOption{option};
        initStyleOption(&itemOption, index);

        if (m_styleSource) {
                const auto probeName = index.column() == 0
                        ? QStringLiteral("metadataKeyColorProbe")
                        : QStringLiteral("metadataValueColorProbe");
                if (const auto* probe = m_styleSource->findChild<QLabel*>(probeName)) {
                        auto palette = itemOption.palette;
                        palette.setColor(QPalette::Text,
                                         probe->palette().color(QPalette::WindowText));
                        palette.setColor(QPalette::HighlightedText,
                                         probe->palette().color(QPalette::WindowText));
                        itemOption.palette = palette;
                }
        }

        QStyledItemDelegate::paint(painter, itemOption, index);
}

} // namespace NfDesktop
