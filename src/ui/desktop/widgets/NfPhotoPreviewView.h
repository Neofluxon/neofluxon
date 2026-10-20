/**
 * File name: NfPhotoPreviewView.h
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

#ifndef NF_PHOTO_PREVIEW_VIEW_H
#define NF_PHOTO_PREVIEW_VIEW_H

#include <QWidget>
#include <QModelIndex>
#include <QPixmap>

class QLabel;
class QResizeEvent;

namespace NfDesktop {

class NfBrowserModel;

class NfPhotoPreviewView : public QWidget
{
    Q_OBJECT

public:
        explicit NfPhotoPreviewView(NfBrowserModel* model, QWidget* parent = nullptr);
        void setModel(NfBrowserModel* model);
        NfBrowserModel* model() const;

public slots:
        void setPhotoIndex(const QModelIndex& index);

protected:
        void connectModel();
        void resizeEvent(QResizeEvent* event) override;

protected slots:
        void updatePreview(const QModelIndex& index = QModelIndex());
        void onDataChanged(const QModelIndex &topLeft,
                           const QModelIndex &bottomRight,
                           const QVector<int> &roles);

private:
        void renderPreview();

        NfBrowserModel *m_model;
        QLabel *m_photoPreview;
        QModelIndex m_previewIndex;
        QPixmap m_sourcePreview;
};

} // NfDesktop

#endif // NF_PREVIEW_MODE_VIEW_H
