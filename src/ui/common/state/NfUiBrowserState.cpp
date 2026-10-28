/**
 * File name: NfUiBrowserState.h
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

#include "NfUiBrowserState.h"
#include "core/NfLogger.h"

using namespace NfCore;

namespace NfUi {

NfUiBrowserState::NfUiBrowserState(QObject* parent)
        : QObject(parent)
        , m_viewMode{ViewMode::Grid}
{
}

NfUiBrowserState::ViewMode NfUiBrowserState::viewMode() const
{
        return m_viewMode;
}

void NfUiBrowserState::setViewMode(ViewMode mode)
{
        if (m_viewMode == mode)
                return;

        m_viewMode = mode;
        emit viewModeChanged(m_viewMode);
}

void NfUiBrowserState::setCurrentPhoto(const NfPhoto& photo)
{
        m_photo = photo;
        emit currentPhotoChanged(m_photo);
}

const NfPhoto& NfUiBrowserState::currentPhoto() const
{
        return m_photo;
}

const NfUiBrowserState::PhotoFormatFilter& NfUiBrowserState::formatFilter() const
{
        return m_formatFilter;
}

void NfUiBrowserState::setFormatFilter(const PhotoFormatFilter& formats)
{
        if (m_formatFilter == formats)
                return;

        m_formatFilter = formats;
        emit formatFilterChanged(m_formatFilter);
}

const NfUiBrowserState::SortFields& NfUiBrowserState::sortingFields() const
{
        return m_sortFields;
}

NfUiBrowserState::SortOrder NfUiBrowserState::sortOrder() const
{
        return m_sortOrder;
}

void NfUiBrowserState::setSortFields(const NfUiBrowserState::SortFields &fields)
{
        if (m_sortFields != fields) {
                m_sortFields = fields;
                emit sortingUpdated();
        }
}

void NfUiBrowserState::enableSortField(NfUiBrowserState::SortField field,
                                       bool b)
{
        NF_LOG_DEBUG("b: " << b);
        if (b ? m_sortFields.insert(field).second
            : m_sortFields.erase(field) > 0) {
                emit sortingUpdated();
        }
}

void NfUiBrowserState::setSortOrder(NfUiBrowserState::SortOrder sortOrder)
{
        if (m_sortOrder != sortOrder) {
                m_sortOrder = sortOrder;
                emit sortingUpdated();
        }
}

} // namespace NfUi
