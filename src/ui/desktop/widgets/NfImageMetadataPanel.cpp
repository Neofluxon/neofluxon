/**
 * File name: NfImageMetadataPanel.cpp
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

#include "NfImageMetadataPanel.h"
#include "NfScrollStack.h"
#include "NfCollapsibleSection.h"
#include "NfPhotoMetadataModel.h"
#include "NfMetadataSectionWidget.h"
#include "NfContext.h"
#include "NfUiState.h"

#include <QVBoxLayout>

#include <utility>

using namespace NfUi;

namespace NfDesktop {

NfImageMetadataPanel::NfImageMetadataPanel(NfContext *ctx,
                                           QWidget* parent)
        : QWidget(parent)
        , m_context{ctx}
        , m_model{new NfPhotoMetadataModel(m_context, this)}
        , m_scrollStack{new NfScrollStack(parent)}
{
        setupUi();
}

void NfImageMetadataPanel::setupUi()
{
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        layout->addWidget(m_scrollStack);

        const auto addSection = [this](const QString& key,
                                       const QString& title,
                                       const QSet<QString>& allowedKeys) {
                auto* section = new NfCollapsibleSection(title);
                section->setExpanded(m_context->uiState->metadataSectionExpanded(key));
                QObject::connect(section,
                                 &NfCollapsibleSection::expandedChanged,
                                 m_context->uiState,
                                 [state = m_context->uiState, key](bool expanded) {
                                         state->setMetadataSectionExpanded(key, expanded);
                                 });
                section->setContent(new NfMetadataSectionWidget(m_model, allowedKeys));
                m_scrollStack->addWidget(section);
        };

        addSection(QStringLiteral("image"), tr("Image"),
                   {tr("Name"), tr("Path"), tr("Size"), tr("Format"), tr("Dimensions")});
        addSection(QStringLiteral("capture"), tr("Capture"),
                   {tr("Date taken"), tr("Camera maker"), tr("Camera model"), tr("Lens"),
                    tr("Aspect ratio"), tr("ISO"), tr("Aperture"), tr("Shutter speed"),
                    tr("Focal length")});
        addSection(QStringLiteral("location"), tr("Location"),
                   {tr("Latitude"), tr("Longitude"), tr("Altitude"), tr("GPS Timestamp"),
                    tr("Positioning Method")});
        addSection(QStringLiteral("organization"), tr("Organization"),
                   {tr("Rating"), tr("Tags"), tr("Collections"), tr("Project")});
        addSection(QStringLiteral("description"), tr("Description"),
                   {tr("Title"), tr("Description"), tr("Author"), tr("Copyright"),
                    tr("License"), tr("Keywords")});
        addSection(QStringLiteral("technical"), tr("Technical"),
                   {tr("Bit depth"), tr("Color space"), tr("Compression"), tr("Orientation"),
                    tr("EXIF"), tr("IPTC"), tr("XMP"), tr("Software"),
                    tr("Modification Date")});
}

} // NfDesktop
