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
#include "NfImageMetadataModel.h"
#include "NfMetadataSectionWidget.h"
#include "NfContext.h"

#include <QVBoxLayout>

using namespace NfUi;

namespace NfDesktop {

NfImageMetadataPanel::NfImageMetadataPanel(NfContext *ctx,
                                           QWidget* parent)
        : QWidget(parent)
        , m_context{ctx}
        , m_model{new NfImageMetadataModel(m_context, this)}
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

        auto* section = new NfCollapsibleSection(tr("Image"));
        auto sectionWidget = new NfMetadataSectionWidget(m_model,
                                                         {tr("Name:"),
                                                          tr("Dimensions:"),
                                                          tr("Size:"),
                                                          tr("Format:"),
                                                          tr("Path:")});
        section->setContent(sectionWidget);
        m_scrollStack->addWidget(section);

        auto w = new QWidget;
        w->setFixedHeight(200);
        section = new NfCollapsibleSection(tr("Histogram"));
        section->setContent(w);
        m_scrollStack->addWidget(section);

        section = new NfCollapsibleSection(tr("Capture"));
        w = new QWidget;
        w->setFixedHeight(200);
        section->setContent(w);
        m_scrollStack->addWidget(section);

        section = new NfCollapsibleSection(tr("Location"));
        w = new QWidget;
        w->setFixedHeight(200);
        section->setContent(w);
        m_scrollStack->addWidget(section);

        w = new QWidget;
        w->setFixedHeight(200);

        section = new NfCollapsibleSection(tr("Organization"));
        section->setContent(w);
        m_scrollStack->addWidget(section);

        w = new QWidget;
        w->setFixedHeight(200);

        section = new NfCollapsibleSection(tr("Description"));
        section->setContent(w);
        m_scrollStack->addWidget(section);

        w = new QWidget;
        w->setFixedHeight(200);

        section = new NfCollapsibleSection(tr("Technical"));
        section->setContent(w);
        m_scrollStack->addWidget(section);
}

} // NfDesktop

