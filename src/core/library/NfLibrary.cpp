/**
 * File name: NfLibrary.cpp
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

#include "NfLibrary.h"
#include "NfLibraryRepresentation.h"

#include <algorithm>

namespace NfCore {

NfLibrary::NfLibrary()
{
}

NfLibrary::~NfLibrary()
{
}

void NfLibrary::setName(const std::string& name)
{
        m_name = name;
}

const std::string& NfLibrary::name() const
{
        return m_name;
}

NfLibraryRepresentation* NfLibrary::addRepresentation()
{
        auto representation = std::make_unique<NfLibraryRepresentation>();
        auto* ptr = representation.get();
        m_representations.push_back(std::move(representation));

        return ptr;
}

void NfLibrary::removeRepresentation(NfLibraryRepresentation* representation)
{
        auto it = std::remove_if(m_representations.begin(),
                                 m_representations.end(),
                                 [representation](const auto& item) {
                                         return item.get() == representation;
        });

        m_representations.erase(it, m_representations.end());
}

const std::vector<std::unique_ptr<NfLibraryRepresentation>>&
NfLibrary::representations() const
{
        return m_representations;
}

} // namespace NfCore
