/**
 * File name: NfImageMetadataPanel.h
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

#ifndef NF_IMAGE_METADATA_PANEL_H
#define NF_IMAGE_METADATA_PANEL_H

#include <QWidget>

namespace NfUi {
class NfContext;
}

namespace NfDesktop {

class NfScrollStack;
class NfCollapsibleSection;
class NfImageMetadataModel;

class NfImageMetadataPanel : public QWidget {
        Q_OBJECT

public:
        explicit NfImageMetadataPanel(NfUi::NfContext* ctx,
                                      QWidget* parent = nullptr);

private:
        void setupUi();

        NfUi::NfContext* m_context;
        NfImageMetadataModel* m_model;
        NfScrollStack* m_scrollStack{nullptr};
        NfCollapsibleSection* m_imageSection{nullptr};
        NfCollapsibleSection* m_histogramSection{nullptr};
        NfCollapsibleSection* m_captureSection{nullptr};
        NfCollapsibleSection* m_technicalSection{nullptr};
};

} // NfDesktop

#endif // NF_IMAGE_METADATA_PANEL_H
