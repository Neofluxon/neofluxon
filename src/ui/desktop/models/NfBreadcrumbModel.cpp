/**
 * File name: NfBreadcrumbModel.cpp
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

#include "NfBreadcrumbModel.h"

namespace NfDesktop {

NfBreadcrumbModel::NfBreadcrumbModel(QObject* parent)
        : QAbstractListModel(parent)
{
}

void NfBreadcrumbModel::setSource(const NfPhotoSource& source)
{
        beginResetModel();
        m_source = source;
        m_pathList = m_source.split();
        endResetModel();
}

int NfBreadcrumbModel::rowCount(const QModelIndex& parent) const
{
        if (parent.isValid())
                return 0;

        return static_cast<int>(m_chain.size());
}

QVariant NfBreadcrumbModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid())
                return {};

        const int row = index.row();
        if (row < 0 || static_cast<size_t>(row) >= m_chain.size())
                return {};

        const auto& source = m_pathList[static_cast<size_t>(row)];
        switch (role) {
        case DisplayNameRole:
                return source.name();
        case PhotoSourceRole:
                return QVariant::fromValue(source.value());
        default:
                return {};
        }
}

QHash<int, QByteArray> NfBreadcrumbModel::roleNames() const
{
        return {
                { DisplayNameRole, "displayName" },
                { PhotoSourceRole, "photoSource" }
        };
}

} // NfDesktop
