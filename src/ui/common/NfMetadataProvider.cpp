/**
 * File name: NfMetadataProvider.cpp
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

#include "NfMetadataProvider.h"
#include "core/NfPhotoLoader.h"
#include "core/NfCache.h"
#include "core/NfPhotoId.h"
#include "core/NfThumbnail.h"
#include "core/NfLogger.h"
#include "core/NeofluxonCore.h"
#include "core/NfImage.h"
#include "NfQPixmap.h"

#include <QTimer>

#include <utility>

using namespace NfCore;

namespace NfUi {

NfMetadataProvider::NfMetadataProvider(NeofluxonCore *core,
                                       QObject* parent)
        : QObject(parent)
        , m_metadataLoader{core->metadataLoader()}
{
        auto timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, &NfMetadataProvider::onTimeout);
        timer->start(100);
}

NfMetadataProvider::~NfMetadataProvider()
{
        NF_LOG_DEBUG("called");
}

void NfMetadataProvider::getMetadata(const NfPhoto &photo) const
{
        m_metadataLoader->requestMetadata(photo);
}

void NfMetadataProvider::onTimeout()
{
        processMetadata();
}

void NfPhotoProvider::processMetadata()
{
        auto metadataList = m_photoLoader->takeMetadata();
        if (metadataList.empty())
                return;

        for (const auto &metadata: metadataList) {
                // TODO: optimize this to send updates only
                // to a perticular requester.
                emit metadataUpdated(metadata.first, metadata.second);
        }
}

} // namespace NfUi
