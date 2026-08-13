/**
 * File name: NfBrowserView.h
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

#ifndef NF_BROWSER_VIEW_H
#define NF_BROWSER_VIEW_H

#include "NfContext.h"

#include <QWidget>

namespace NfUi {
 class NfUiBrowserState;
}

class QVBoxLayout;

namespace NfDesktop {

class NfBrowserModel;
class NfBreadcrumbView;
class NfThumbnailsView;
class NfPhotoPreviewView;

class NfBrowserView : public QWidget
{
        Q_OBJECT

public:
        explicit NfBrowserView(NfUi::NfUiBrowserState *state,
                               NfBrowserModel *model,
                               QWidget* parent = nullptr);

public slots:
        void showGridView();
        void showPreviewView();

protected slots:
        void updateView();

protected:
        void keyPressEvent(QKeyEvent *event) override;

private:
        NfUi::NfUiBrowserState *m_state;
        NfBrowserModel* m_model;
        QVBoxLayout* m_mainLayout{nullptr};
        NfBreadcrumbView *m_breadcrumbBar{nullptr};
        NfThumbnailsView* m_thumbnailsView{nullptr};
        NfPhotoPreviewView* m_photoPreviewView{nullptr};
};

} // namespace NfDesktop

#endif // NF_BROWSER_VIEW_H
