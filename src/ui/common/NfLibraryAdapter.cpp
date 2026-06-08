/**
 * File name: NfLibraryAdapter.cpp
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

#include "NfLibraryAdapter.h"
#include "core/library/NfLibraryManager.h"
#include "core/library/NfLibrary.h"
#include "core/NfLogger.h"

using namespace NfCore;

namespace NfUi {

NfLibraryAdapter::NfLibraryAdapter(NfLibraryManager *library,
                                   QObject* parent)
        : QObject(parent)
        , m_library{library}
{
}

NfLibraryAdapter::~NfLibraryAdapter()
{
        NF_LOG_DEBUG("called");
}

void NfLibraryAdapter::addLibrary(std::string_view name)
{
        m_library->addLibrary(name);
}

void NfLibraryAdapter::importPath(const std::filesystem::path &path, uint64_t libraryId)
{
        m_library->importPath(path, libraryId);
}

std::vector<std::unique_ptr<NfLibrary>> NfLibraryAdapter::libraries() const
{
        return m_library->libraries();
}

} // namespace NfUi
