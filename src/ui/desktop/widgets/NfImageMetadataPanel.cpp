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

#include <QVBoxLayout>

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

        // Image
        auto* section = new NfCollapsibleSection(tr("Image"));
        auto sectionWidget = new NfMetadataSectionWidget(m_model,
                                                         {tr("Name"),
                                                          tr("Path"),
                                                          tr("Size"),
                                                          tr("Format"),
                                                          tr("Dimensions")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        // Capture section
        section = new NfCollapsibleSection(tr("Capture"));
        sectionWidget = new NfMetadataSectionWidget(m_model,
                                                    {tr("Date taken"),
                                                     tr("Camera maker"),
                                                     tr("Camera model"),
                                                     tr("Lens"),
                                                     tr("ISO"),
                                                     tr("Aperture"),
                                                     tr("Shutter speed"),
                                                     tr("Focal length")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        // Location section
        section = new NfCollapsibleSection(tr("Location"));
        sectionWidget = new NfMetadataSectionWidget(m_model,
                                                    {tr("Latitude"),
                                                     tr("Longitude"),
                                                     tr("Altitude"),
                                                     tr("GPS Timestamp"),
                                                     tr("Positioning Method")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        // Organization section
        section = new NfCollapsibleSection(tr("Organization"));
        sectionWidget = new NfMetadataSectionWidget(m_model,
                                                    {tr("Rating"),
                                                     tr("Tags"),
                                                     tr("Collections"),
                                                     tr("Project")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        // Description section
        section = new NfCollapsibleSection(tr("Description"));
        sectionWidget = new NfMetadataSectionWidget(m_model,
                                                    {tr("Title"),
                                                     tr("Description"),
                                                     tr("Author"),
                                                     tr("Copyright"),
                                                     tr("License"),
                                                     tr("Keywords")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        // Technical section
        section = new NfCollapsibleSection(tr("Technical"));
        sectionWidget = new NfMetadataSectionWidget(m_model,
                                                    {tr("Bit depth"),
                                                     tr("Color space"),
                                                     tr("Compression"),
                                                     tr("Orientation"),
                                                     tr("EXIF"),
                                                     tr("IPTC"),
                                                     tr("XMP"),
                                                     tr("Software"),
                                                     tr("Modification Date")});

        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);
}

} // NfDesktop

