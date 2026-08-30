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

#include <QVBoxLayout>

namespace NfDesktop {

NfImageMetadataPanel::NfImageMetadataPanel(QWidget* parent)
        : QWidget(parent)
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

        auto* imageSection = new NfCollapsibleSection("Image");
        imageSection->setContent(new QWidget/*NfImageInfoWidget(m_model)*/);
        m_scrollStack->addWidget(imageSection);

        auto* histogramSection = new NfCollapsibleSection("Histogram");
        histogramSection->setContent(new QWidget/*NfHistogramWidget(m_model)*/);
        m_scrollStack->addWidget(histogramSection);

        auto* captureSection = new NfCollapsibleSection("Capture");
        captureSection->setContent(new QWidget/*new NfCaptureInfoWidget(m_model)*/);
        m_scrollStack->addWidget(captureSection);

        auto* locationSection = new NfCollapsibleSection("Location");
        locationSection->setContent(new QWidget/*new NfLocationInfoWidget(m_model)*/);
        m_scrollStack->addWidget(locationSection);

        auto* organizationSection = new NfCollapsibleSection("Organization");
        organizationSection->setContent(new QWidget/*new NfOrganizationInfoWidget(m_model)*/);
        m_scrollStack->addWidget(organizationSection);

        auto* descriptionSection = new NfCollapsibleSection("Description");
        descriptionSection->setContent(new QWidget/*new NfDescriptionInfoWidget(m_model)*/);
        m_scrollStack->addWidget(descriptionSection);

        auto* technicalSection = new NfCollapsibleSection("Technical");
        technicalSection->setContent(new QWidget/*new NfTechnicalInfoWidget(m_model)*/);
        m_scrollStack->addWidget(technicalSection);
}

} // NfDesktop

