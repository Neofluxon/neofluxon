/**
 * File name: NfLibraryModel.cpp
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

#include "NfLibraryModel.h"
#include "NfContext.h"
#include "NfBrowserModel.h"
#include "core/library/NfLibraryQuery.h"
#include "core/library/NfLibraryPhotoSource.h"

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfLibraryModel::NfLibraryModel(NfLibraryContext ctx, QObject *parent)
        : QObject(parent)
        , m_context{std::move(ctx)}
        , m_browserModel{new NfBrowserModel(m_context.prent(), this)}
{
}

void NfLibraryModel::setQuery(const NfLibraryQuery &query)
{
        m_browserModel->setSource(NfLibraryPhotoSource{query});
}

NfBrowserModel* NfLibraryModel::browser() const
{
        return m_browserModel;
}

} // namespace NfDesktop
