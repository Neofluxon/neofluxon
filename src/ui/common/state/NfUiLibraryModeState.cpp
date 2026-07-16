/**
 * File name: NfUiLibraryModeState.h
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

#include "NfUiLibraryModeState.h"
#include "NfUiBrowserState.h"

using namespace NfCore;

namespace NfUi {

NfUiLibraryModeState::NfUiLibraryModeState(QObject* parent)
    : QObject(parent)
    , m_browserState{new NfUiBrowserState(this)}
{
}

void NfUiLibraryModeState::setQuery(const NfLibraryQuery& q)
{
        if (m_query == q)
                return;

        m_query = q;
        emit queryChanged(m_query);
}

const NfLibraryQuery& NfUiLibraryModeState::query() const
{
        return m_query;
}

NfUiBrowserState* NfUiLibraryModeState::browser() const
{
        return m_browserState;
}

} // namespace NfUi
