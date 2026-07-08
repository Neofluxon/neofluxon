/**
 * File name: NfFolderModel.cpp
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

#include "NfFolderModel.h"
#include "NfBrowserModel.h"

using namespace NfUi;

namespace NfDesktop {

NfFolderModel::NfFolderModel(NfContext *ctx, QObject *parent)
        : QObject(parent)
        , m_context{ctx}
        , m_browserModel{new NfBrowserModel(m_context, this)}
{
}

void NfFolderModel::setPath(const std::filesystem::path& path)
{
        m_browserModel->setSource(path);
}

const std::filesystem::path& NfFolderModel::getPath() const
{
        return m_browserModel->getSurce().;
}

NfBrowserModel* NfFolderModel::browser() const
{
        return m_browserModel;
}

} // namespace NfDesktop
