/**
 * File name: NfMetadataProvider.h
 * Project: Neofluxon (a photography workflow software)
 *
 * Copyright (C) 2026 Iurie Nistor
 *
 * This file is part of Neofluxon.
 *
 * Neofluxon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef NF_METADATA_PROVIDER_H
#define NF_METADATA_PROVIDER_H

#include "core/NfPhotoId.h"
#include "core/NfPhoto.h"
#include "core/NfPhotoMetadata.h"

#include <QObject>

#include <cstdint>
#include <vector>

namespace NfCore {
class NeofluxonCore;
class NfPhotoMetadataLoader;
}

namespace NfUi {

class NfMetadataProvider : public QObject
{
        Q_OBJECT

public:
        explicit NfMetadataProvider(NfCore::NeofluxonCore *core,
                                    QObject* parent = nullptr);
        ~NfMetadataProvider();
        void getMetadata(const NfCore::NfPhoto &photo) const;

signals:
        void metadataUpdated(const NfCore::NfPhotoId &photoId,
                             const NfCore::NfPhotoMetadata &metadata);

private slots:
        void onTimeout();

private:
        void processMetadata();

        NfCore::NfPhotoMetadataLoader *m_metadataLoader;
};

} // namespace NfUi

#endif // NF_METADATA_PROVIDER_H
