/**
 * File name: NfMetadataFilterProxy.cpp
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

#include "NfMetadataFilterProxy.h"
#include "NfImageMetadataModel.h"

namespace NfDesktop {

NfMetadataFilterProxy::NfMetadataFilterProxy(const QSet<QString>& allowedKeys,
                                             QObject* parent)
        : QSortFilterProxyModel(parent)
        , m_allowedKeys{allowedKeys}
{
}

void NfMetadataFilterProxy::setAllowedKeys(const QSet<QString>& allowedKeys)
{
        m_allowedKeys = allowedKeys;
        invalidateFilter();
}

QSet<QString> NfMetadataFilterProxy::allowedKeys() const
{
        return m_allowedKeys;
}

bool NfMetadataFilterProxy::filterAcceptsRow(int source, const QModelIndex& parent) const
{
        if (!sourceModel())
                return false;

        auto index = sourceModel()->index(source, 0, parent);

        // Donot show category header
        if (index.data(NfImageMetadataModel::IsHeaderRole).toBool())
                return false;

        auto key = index.data(Qt::DisplayRole).toString();

        return m_allowedKeys.contains(key);
}

} // namespace NfDesktop
