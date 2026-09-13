/**
 * File name: NfImageMetadataModel.cpp
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

#include "NfPhotoMetadataModel.h"
#include "NfContext.h"
#include "NfMetadataProvider.h"

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfPhotoMetadataModel::NfPhotoMetadataModel(NfContext* ctx, QObject* parent)
        : QAbstractTableModel(parent)
        , m_context{ctx}
{
        m_context->metadataProvider()->subscribe(this);
}

int NfPhotoMetadataModel::rowCount(const QModelIndex& parent) const
{
        if (parent.isValid())
                return 0;

        return m_items.size();
}

int NfPhotoMetadataModel::columnCount(const QModelIndex& parent) const
{
        if (parent.isValid())
                return 0;

        return 2;
}

QVariant NfPhotoMetadataModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid() || index.row() < 0
            || index.row() >= m_items.size())
                return QVariant();

        const auto& item = m_items.at(index.row());
        if (role == Qt::DisplayRole) {
                if (index.column() == 0)
                        return item.key;
                else if (index.column() == 1)
                        return item.value;
        }  else if (role == IsHeaderRole) {
                return item.isHeader;
        }

        return QVariant();
}

QVariant NfPhotoMetadataModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const
{
        if (role != Qt::DisplayRole)
                return QVariant();

        if (orientation == Qt::Horizontal) {
                if (section == 0)
                        return tr("Property");
                if (section == 1)
                        return tr("Value");
        }

        return QVariant();
}

void NfPhotoMetadataModel::setPhoto(const NfCore::NfPhoto &photo)
{
        if (m_photo == photo)
                return;

        beginResetModel();
        m_photo = photo;
        setupMetadata(m_photoProvider->getMetadata(photo));
        endResetModel();
}

void NfPhotoMetadataModel::metadataUpdated(const NfPhotoMetadata &metadata)
{
        if (m_photo.id() != metadata.photoId)
                return;

        beginResetModel();
        setupMetadata(metadata);
        endResetModel();
}

void NfPhotoMetadataModel::setupMetadata(const NfCore::NfPhotoMetadata& metadata)
{
        m_items.clear();

        // File
        m_items.append({tr("File"), {}, true});
        m_items.append({tr("Name"), m_photo.fileName()});
        m_items.append({tr("Path"), m_photo.path()});
        m_items.append({tr("Size"), formatFileSize(metadata.fileSize)});

        // Image
        m_items.append({tr("Image"), {}, true});
        m_items.append({tr("Dimensions"),
                        QStringLiteral("%1 × %2")
                        .arg(metadata.width)
                        .arg(metadata.height)});
        m_items.append({tr("Format"),
                        QString::fromStdString(metadata.format)});

        // Capture
        m_items.append({tr("Capture"), {}, true});
        m_items.append({tr("Date taken"),
                        formatDate(metadata.dateTaken)});
        m_items.append({tr("Camera maker"),
                        QString::fromStdString(metadata.cameraMaker)});
        m_items.append({tr("Camera model"),
                        QString::fromStdString(metadata.cameraModel)});
        m_items.append({tr("Lens"),
                        QString::fromStdString(metadata.lens)});
        m_items.append({tr("ISO"),
                        metadata.iso >= 0
                        ? QString::number(metadata.iso)
                        : QString()});
        m_items.append({tr("Aperture"),
                        metadata.aperture > 0.0
                        ? QStringLiteral("f/%1")
                        .arg(metadata.aperture, 0, 'f', 1)
                        : QString()});
        m_items.append({tr("Shutter speed"),
                        formatShutterSpeed(metadata.shutterSpeed)});
        m_items.append({tr("Focal length"),
                        metadata.focalLength > 0.0
                        ? QStringLiteral("%1 mm")
                        .arg(metadata.focalLength, 0, 'f', 0)
                        : QString()});

        // Location
        m_items.append({tr("Location"), {}, true});
        if (metadata.hasGps) {
                m_items.append({tr("Latitude"),
                                QString::number(metadata.latitude, 'f', 6)});
                m_items.append({tr("Longitude"),
                                QString::number(metadata.longitude, 'f', 6)});
                m_items.append({tr("Altitude"),
                                QStringLiteral("%1 m")
                                .arg(metadata.altitude, 0, 'f', 1)});
        }

        // Organization
        m_items.append({tr("Organization"), {}, true});
        m_items.append({tr("Rating"),
                        QString::number(metadata.rating)});
        m_items.append({tr("Tags"),
                        QString::fromStdString(metadata.tags)});

        // Description
        m_items.append({tr("Description"), {}, true});
        m_items.append({tr("Title"),
                        QString::fromStdString(metadata.title)});
        m_items.append({tr("Description"),
                        QString::fromStdString(metadata.description)});
        m_items.append({tr("Author"),
                        QString::fromStdString(metadata.author)});
        m_items.append({tr("License"),
                        QString::fromStdString(metadata.license)});

        // Technical
        m_items.append({tr("Technical"), {}, true});
        m_items.append({tr("Bit depth"),
                        metadata.bitDepth > 0
                        ? QString::number(metadata.bitDepth)
                        : QString()});
        m_items.append({tr("Color space"),
                        QString::fromStdString(metadata.colorSpace)});
        m_items.append({tr("Orientation"),
                        QString::number(metadata.orientation)});
        m_items.append({tr("EXIF"),
                        metadata.hasExif ? tr("Yes") : tr("No")});
}

} // namespace NfDesktop
