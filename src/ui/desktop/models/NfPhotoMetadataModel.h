/**
 * File name: NfPhotoMetadataModel.h
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

#ifndef NF_PHOTO_METADATA_MODEL_H
#define NF_PHOTO_METADATA_MODEL_H

#include "core/NfPhoto.h"

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

#include <chrono>

namespace NfCore {
class NfPhotoId;
class NfPhotoMetadata;
}

namespace NfUi {
class NfContext;
class NfPhotoProvider;
}

namespace NfDesktop {

class NfPhotoMetadataModel : public QAbstractTableModel
{
        Q_OBJECT

public:
        enum Roles {
                IsHeaderRole = Qt::UserRole + 1
        };

        explicit NfPhotoMetadataModel(NfUi::NfContext* ctx, QObject* parent = nullptr);
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
        void setPhoto(const NfCore::NfPhoto &photo);

protected slots:
        void metadataUpdated(const NfCore::NfPhotoId &photoId,
                             const NfCore::NfPhotoMetadata &metadata);

protected:
        void setupMetadata(const NfCore::NfPhotoMetadata& metadata);
        static QString formatFileSize(std::uint64_t size);
        static QString formatDate( const std::chrono::system_clock::time_point& date);
        QString formatShutterSpeed(double seconds);

private:

        struct MetadataItem {
                QString key;
                QString value;
                bool isHeader{false};
                MetadataItem(QString key, QString value, bool isHeader = false)
                        : key{std::move(key)}
                        , value{std::move(value)}
                        , isHeader{isHeader}
                {
                }
        };

        NfUi::NfContext* m_context;
        NfUi::NfPhotoProvider *m_photoProvider;
        NfCore::NfPhoto m_photo;
        QVector<MetadataItem> m_items;
};

} // namespace NfDesktop

#endif // NF_PHOTO_METADATA_MODEL_H
