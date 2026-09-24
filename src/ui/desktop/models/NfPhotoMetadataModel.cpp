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
#include "NfPhotoId.h"
#include "NfPhotoMetadata.h"
#include "NfUiState.h"
#include "NfUiFolderModeState.h"
#include "NfUiLibraryModeState.h"
#include "NfUiBrowserState.h"
#include "core/NfLogger.h"

#include <QDateTime>

using namespace NfCore;
using namespace NfUi;

namespace NfDesktop {

NfPhotoMetadataModel::NfPhotoMetadataModel(NfContext* ctx, QObject* parent)
        : QAbstractTableModel(parent)
        , m_context{ctx}
{
        QObject::connect(m_context->metadataProvider,
                         &NfMetadataProvider::metadataUpdated,
                         this,
                         &NfPhotoMetadataModel::metadataUpdated);

        QObject::connect(m_context->uiState->folderModeState()->browser(),
                         &NfUiBrowserState::currentPhotoChanged,
                         this,
                         &NfPhotoMetadataModel::setPhoto);

        QObject::connect(m_context->uiState->libraryModeState()->browser(),
                         &NfUiBrowserState::currentPhotoChanged,
                         this,
                         &NfPhotoMetadataModel::setPhoto);
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
        if (m_photo.id() == photo.id())
                return;

        NF_LOG_DEBUG("set photo: " << photo.path());

        m_photo = photo;
        // TODO: implement metadata chache
        m_context->metadataProvider->getMetadata(m_photo);
}

void NfPhotoMetadataModel::metadataUpdated(const NfPhotoId &photoId,
                                           const NfPhotoMetadata &metadata)
{
        NF_LOG_DEBUG("photoId: " << photoId.isValid());

        if (m_photo.id() != photoId)
                return;

        beginResetModel();
        setupMetadata(metadata);
        endResetModel();
}

void NfPhotoMetadataModel::setupMetadata(const NfCore::NfPhotoMetadata& metadata)
{
        m_items.clear();

        NF_LOG_DEBUG("photo name: " << m_photo.name());

        // Image
        m_items.emplaceBack(tr("Name"),
                            QString::fromStdString(m_photo.name()));
        m_items.emplaceBack(tr("Path"),
                            QString::fromStdString(m_photo.path()));
        m_items.emplaceBack(tr("Size"),
                            formatFileSize(metadata.fileSize));
        m_items.emplaceBack(tr("Format"),
                            QString::fromStdString(metadata.format));
        m_items.emplaceBack(tr("Dimensions"),
                            QStringLiteral("%1 × %2")
                            .arg(metadata.width)
                            .arg(metadata.height));

        // Capture
        m_items.emplaceBack(tr("Date taken"),
                            formatDate(metadata.dateTaken));
        m_items.emplaceBack(tr("Camera maker"),
                            QString::fromStdString(metadata.cameraMaker));
        m_items.emplaceBack(tr("Camera model"),
                            QString::fromStdString(metadata.cameraModel));
        m_items.emplaceBack(tr("Lens"),
                            QString::fromStdString(metadata.lens));
        m_items.emplaceBack(tr("ISO"),
                            metadata.iso >= 0
                            ? QString::number(metadata.iso)
                            : QString{});
        m_items.emplaceBack(tr("Aperture"),
                            metadata.aperture > 0.0
                            ? QStringLiteral("f/%1")
                            .arg(metadata.aperture, 0, 'f', 1)
                            : QString{});
        m_items.emplaceBack(tr("Shutter speed"),
                            formatShutterSpeed(metadata.shutterSpeed));
        m_items.emplaceBack(tr("Focal length"),
                            metadata.focalLength > 0.0
                            ? QStringLiteral("%1 mm")
                            .arg(metadata.focalLength, 0, 'f', 0)
                            : QString{});

        // Location
        if (metadata.hasGps) {
                m_items.emplaceBack(tr("Latitude"),
                                    QString::number(metadata.latitude, 'f', 6));
                m_items.emplaceBack(tr("Longitude"),
                                    QString::number(metadata.longitude, 'f', 6));
                m_items.emplaceBack(tr("Altitude"),
                                    QStringLiteral("%1 m")
                                    .arg(metadata.altitude, 0, 'f', 1));
        }

        // Organization
        m_items.emplaceBack(tr("Rating"),
                            QString::number(metadata.rating));
        m_items.emplaceBack(tr("Tags"),
                            QString::fromStdString(metadata.tags));

        // Description
        m_items.emplaceBack(tr("Title"),
                            QString::fromStdString(metadata.title));
        m_items.emplaceBack(tr("Description"),
                            QString::fromStdString(metadata.description));
        m_items.emplaceBack(tr("Author"),
                            QString::fromStdString(metadata.author));
        m_items.emplaceBack(tr("License"),
                            QString::fromStdString(metadata.license));

        // Technical
        m_items.emplaceBack(tr("Bit depth"),
                            metadata.bitDepth > 0
                            ? QString::number(metadata.bitDepth)
                            : QString{});
        m_items.emplaceBack(tr("Color space"),
                            QString::fromStdString(metadata.colorSpace));
        m_items.emplaceBack(tr("Orientation"),
                            QString::number(metadata.orientation));
        m_items.emplaceBack(tr("EXIF"),
                            metadata.hasExif ? tr("Yes") : tr("No"));
}

QString NfPhotoMetadataModel::formatFileSize(std::uint64_t size)
{
        if (size < 1024)
                return QStringLiteral("%1 B").arg(size);

        if (size < 1024 * 1024)
                return QStringLiteral("%1 KB")
                        .arg(size / 1024.0, 0, 'f', 1);

        if (size < 1024ULL * 1024 * 1024)
                return QStringLiteral("%1 MB")
                        .arg(size / (1024.0 * 1024.0), 0, 'f', 1);

        return QStringLiteral("%1 GB")
                .arg(size / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

QString NfPhotoMetadataModel::formatDate(const std::chrono::system_clock::time_point& date)
{
        if (date == std::chrono::system_clock::time_point{})
                return {};

        const auto time = std::chrono::system_clock::to_time_t(date);
        const auto localTime = QDateTime::fromSecsSinceEpoch(time);

        return localTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
}

QString NfPhotoMetadataModel::formatShutterSpeed(double seconds)
{
        if (seconds <= 0.0)
                return {};

        if (seconds >= 1.0)
                return QStringLiteral("%1 s")
                        .arg(seconds, 0, 'f', seconds < 10.0 ? 1 : 0);

        const auto denominator = std::round(1.0 / seconds);

        return QStringLiteral("1/%1 s")
                .arg(static_cast<int>(denominator));
}

} // namespace NfDesktop
