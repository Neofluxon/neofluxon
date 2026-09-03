/**
 * File name: NfImageMetadataModel.h
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

#ifndef NF_IMAGE_METADATA_MODEL_H
#define NF_IMAGE_METADATA_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

namespace NfDesktop {

struct NfMetadataItem {
        QString key;
        QString value;
        bool isHeader{false};
};

class NfImageMetadataModel : public QAbstractTableModel
{
        Q_OBJECT

public:
        enum Roles {
                IsHeaderRole = Qt::UserRole + 1
        };

        explicit NfImageMetadataModel(QObject* parent = nullptr);
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
        void setMetadata(const QVector<NfMetadataItem>& items);
        void clear();

private:
        QVector<NfMetadataItem> m_items;
};

} // namespace NfDesktop

#endif // NF_IMAGE_METADATA_MODEL_H
