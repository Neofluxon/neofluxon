/**
 * File name: NfBreadcrumbView.h
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

#ifndef NF_BREADCRUMB_VIEW_H
#define NF_BREADCRUMB_VIEW_H

#include <QWidget>
#include <QModelIndex>
#include <QPersistentModelIndex>

class QHBoxLayout;
class QToolButton;
class QAbstractItemModel;

namespace NfDesktop {

class NfBreadcrumbView : public QWidget
{
        Q_OBJECT

 public:
        explicit NfBreadcrumbView(QWidget* parent = nullptr);
        ~NfBreadcrumbView() override = default;
        void setModel(QAbstractItemModel* model);
        QAbstractItemModel* model() const { return m_model; }
        void setCurrentIndex(const QModelIndex& index);
        QModelIndex currentIndex() const { return m_currentIndex; }
        void setDisplayRole(int role);
        void setIconRole(int role);

 signals:
        void activated(const QModelIndex& index);
        void leftButtonClicked();
        void rightButtonClicked();

private slots:
        void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
        void onModelReset();

private:
        void rebuildLayout();
        void clearLayout();
        void updateArrowStates();

        QToolButton* m_leftButton;
        QToolButton* m_rightButton;
        QWidget* m_container;
        QHBoxLayout* m_containerLayout;
        QAbstractItemModel* m_model{nullptr};
        QPersistentModelIndex m_currentIndex;
        int m_displayRole{Qt::DisplayRole};
        int m_iconRole{Qt::DecorationRole};
};

} // namespace NfDesktop

#endif // NF_BREADCRUMB_VIEW_H
