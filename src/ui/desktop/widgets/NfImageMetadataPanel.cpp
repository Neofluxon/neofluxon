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

        auto* w = new QWidget;
        w->setFixedHeight(200);

        auto* imageSection = new NfCollapsibleSection("Image");
        imageSection->setContent(w/*NfImageInfoWidget(m_model)*/);
        m_scrollStack->addWidget(imageSection);

        w = new QWidget;
        w->setFixedHeight(200);

        auto* histogramSection = new NfCollapsibleSection("Histogram");
        histogramSection->setContent(w/*NfHistogramWidget(m_model)*/);
        m_scrollStack->addWidget(histogramSection);

        auto* captureSection = new NfCollapsibleSection("Capture");
        w = new QWidget;
        w->setFixedHeight(200);
        captureSection->setContent(w/*new NfCaptureInfoWidget(m_model)*/);
        m_scrollStack->addWidget(captureSection);

        auto* locationSection = new NfCollapsibleSection("Location");
        w = new QWidget;
        w->setFixedHeight(200);
        locationSection->setContent(w/*new NfLocationInfoWidget(m_model)*/);
        m_scrollStack->addWidget(locationSection);

        w = new QWidget;
        w->setFixedHeight(200);

        auto* organizationSection = new NfCollapsibleSection("Organization");
        organizationSection->setContent(w/*new NfOrganizationInfoWidget(m_model)*/);
        m_scrollStack->addWidget(organizationSection);

        w = new QWidget;
        w->setFixedHeight(200);

        auto* descriptionSection = new NfCollapsibleSection("Description");
        descriptionSection->setContent(w/*new NfDescriptionInfoWidget(m_model)*/);
        m_scrollStack->addWidget(descriptionSection);

        w = new QWidget;
        w->setFixedHeight(200);

        auto* technicalSection = new NfCollapsibleSection("Technical");
        technicalSection->setContent(w/*new NfTechnicalInfoWidget(m_model)*/);
        m_scrollStack->addWidget(technicalSection);
}

} // NfDesktop

