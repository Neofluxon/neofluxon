/**
 * File name: NfThumbnailsView.h
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

#ifndef NF_THUBNAILS_VIEW_H
#define NF_THUBNAILS_VIEW_H

#include <QListView>

class QWheelEvent;
class QResizeEvent;
class QMouseEvent;

namespace NfDesktop {

class NfThumbnailsView : public QListView
{
    Q_OBJECT

public:
        enum class LayoutMode {
                GridView,
                FilmstripView
        };

        explicit NfThumbnailsView(QWidget* parent = nullptr);

        void setLayoutMode(LayoutMode mode);
        LayoutMode layoutMode() const;

        void setThumbnailSize(const QSize &thumSize);
        const QSize& thumbnailSize() const;

signals:
        void photoActivated(const QModelIndex& index);
        void photoHovered(const QModelIndex& index);

protected:
        void resizeEvent(QResizeEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void keyPressEvent(QKeyEvent *event) override;
        void onScrollChanged();

private:
        void updateLayout();

private:
        LayoutMode m_layoutMode;
        QSize m_thumbnailSize;
};

} // namespace NfDesktop

#endif // NF_THUBNAILS_VIEW_H
