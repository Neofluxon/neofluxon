/**
 * File name: NfImageMetadataModel.cpp
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

#include "NfImageMetadataModel.h"

namespace NfDesktop {

NfImageMetadataModel::NfImageMetadataModel(QObject* parent)
        : QAbstractTableModel(parent)
{
}

int NfImageMetadataModel::rowCount(const QModelIndex& parent) const
{
        if (parent.isValid())
                return 0;

        return m_items.size();
}

int NfImageMetadataModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant NfImageMetadataModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() < 0
            || index.row() >= m_items.size())
                return QVariant();

        const auto& item = m_items.at(index.row());
        if (role == Qt::DisplayRole) {
                if (index.column() == 0)
                        return item.key;
                else if (index.column() == 1)
                        return item.value;
        }  else if (role == IsHeaderRole) {
                return item.isHeader;
        }

        return QVariant();
}

QVariant NfImageMetadataModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const
{
        if (role != Qt::DisplayRole)
                return QVariant();

        if (orientation == Qt::Horizontal) {
                if (section == 0)
                        return tr("Property");
                if (section == 1)
                        return tr("Value");
        }

        return QVariant();
}

void NfImageMetadataModel::setMetadata(const QVector<NfMetadataItem>& items)
{
        beginResetModel();
        m_items = items;
        endResetModel();
}

void NfImageMetadataModel::clear()
{
        beginResetModel();
        m_items.clear();
        endResetModel();
}

} // namespace NfDesktop
