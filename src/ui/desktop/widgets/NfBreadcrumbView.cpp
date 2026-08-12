/**
 * File name: NfBreadcrumbView.cpp
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

#include "NfBreadcrumbView.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QAbstractItemModel>

namespace NfDesktop {

NfBreadcrumbView::NfBreadcrumbView(QWidget* parent)
    : QWidget(parent)
{
        auto* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(2);

        m_leftButton = new QToolButton(this);
        m_leftButton->setArrowType(Qt::LeftArrow);
        QObject::connect(m_leftButton, &QToolButton::clicked, this, [this]() {
                if (!m_model || !m_currentIndex.isValid())
                        return;

                int currentRow = m_currentIndex.row();
                if (currentRow > 0) {
                        auto prevIndex = m_model->index(currentRow - 1,
                                                        0,
                                                        m_currentIndex.parent());
                        setCurrentIndex(prevIndex);
                        emit activated(prevIndex);
                }
                emit leftButtonClicked();
        });

        m_rightButton = new QToolButton(this);
        m_rightButton->setArrowType(Qt::RightArrow);
        QObject::connect(m_rightButton, &QToolButton::clicked, this, [this]() {
                if (!m_model || !m_currentIndex.isValid())
                        return;

                int currentRow = m_currentIndex.row();
                int totalRows = m_model->rowCount(m_currentIndex.parent());
                if (currentRow < totalRows - 1) {
                        auto nextIndex = m_model->index(currentRow + 1,
                                                        0,
                                                        m_currentIndex.parent());
                        setCurrentIndex(nextIndex);
                        emit activated(nextIndex);
                }
                emit rightButtonClicked();
        });

        m_container = new QWidget(this);
        m_containerLayout = new QHBoxLayout(m_container);
        m_containerLayout->setContentsMargins(0, 0, 0, 0);
        m_containerLayout->setSpacing(2);
        m_containerLayout->setAlignment(Qt::AlignLeft);

        mainLayout->addWidget(m_leftButton);
        mainLayout->addWidget(m_container, 1);
        mainLayout->addWidget(m_rightButton);

        updateArrowStates();
}

void NfBreadcrumbView::setModel(QAbstractItemModel* model)
{
        if (m_model == model)
                return;

        if (m_model) {
                QObject::disconnect(m_model,
                                    &QAbstractItemModel::dataChanged,
                                    this,
                                    &NfBreadcrumbView::onModelDataChanged);
                QObject::disconnect(m_model,
                                    &QAbstractItemModel::modelReset,
                                    this,
                                    &NfBreadcrumbView::onModelReset);
                QObject::disconnect(m_model,
                                    &QAbstractItemModel::rowsInserted,
                                    this,
                                    &NfBreadcrumbView::onModelReset);
                QObject::disconnect(m_model,
                                    &QAbstractItemModel::rowsRemoved,
                                    this,
                                    &NfBreadcrumbView::onModelReset);
        }

        m_model = model;
        if (m_model) {
                QObject::connect(m_model,
                                 &QAbstractItemModel::dataChanged,
                                 this,
                                 &NfBreadcrumbView::onModelDataChanged);
                QObject::connect(m_model,
                                 &QAbstractItemModel::modelReset,
                                 this,
                                 &NfBreadcrumbView::onModelReset);
                QObject::connect(m_model,
                                 &QAbstractItemModel::rowsInserted,
                                 this,
                                 &NfBreadcrumbView::onModelReset);
                QObject::connect(m_model,
                                 &QAbstractItemModel::rowsRemoved,
                                 this,
                                 &NfBreadcrumbView::onModelReset);
        }

        m_currentIndex = QModelIndex();
        rebuildLayout();
}

void NfBreadcrumbView::setCurrentIndex(const QModelIndex& index)
{
        if (m_currentIndex == index)
                return;

        m_currentIndex = index;
        rebuildLayout();
}

void NfBreadcrumbView::setDisplayRole(int role)
{
        m_displayRole = role;
        rebuildLayout();
}

void NfBreadcrumbView::setIconRole(int role)
{
        m_iconRole = role;
        rebuildLayout();
}

void NfBreadcrumbView::clearLayout()
{
        QLayoutItem* child;
        while ((child = m_containerLayout->takeAt(0)) != nullptr) {
                if (child->widget())
                        child->widget()->deleteLater();
                delete child;
        }
}

void NfBreadcrumbView::updateArrowStates()
{
        if (!m_model || !m_currentIndex.isValid()) {
                m_leftButton->setEnabled(false);
                m_rightButton->setEnabled(false);
                return;
        }

        int currentRow = m_currentIndex.row();
        int totalRows = m_model->rowCount(m_currentIndex.parent());

        m_leftButton->setEnabled(currentRow > 0);
        m_rightButton->setEnabled(currentRow < totalRows - 1);
}

void NfBreadcrumbView::rebuildLayout()
{
        clearLayout();

        if (!m_model || !m_currentIndex.isValid())
                return;

        int activeRow = m_currentIndex.row();
        auto parentIdx = m_currentIndex.parent();
        int totalRows = m_model->rowCount(parentIdx);

        // Left arrow button
        auto* leftButton = new QToolButton(m_container);
        leftButton->setArrowType(Qt::LeftArrow);
        leftButton->setEnabled(activeRow > 0);
        QObect::connect(leftButton,
                        &QToolButton::clicked,
                        this,
                        [this, activeRow, parentIdx]() {
                                if (activeRow > 0) {
                                        auto prevIndex = m_model->index(activeRow - 1, 0, parentIdx);
                                        setCurrentIndex(prevIndex);
                                        emit activated(prevIndex);
                                }
                                emit leftButtonClicked();
                        });
        m_containerLayout->addWidget(leftButton);

        // Breadcrump items
        for (int r = 0; r < totalRows; ++r) {
                        auto idx = m_model->index(r, 0, parentIdx);
                        if (!idx.isValid())
                                continue;

                        auto* button = new QToolButton(m_container);
                        const auto text = m_model->data(idx, m_displayRole).toString();
                        const auto icon = m_model->data(idx, m_iconRole).value<QIcon>();

                        button->setText(text);
                        if (!icon.isNull()) {
                                button->setIcon(icon);
                                button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                        }

                        if (r == activeRow) {
                                button->setCheckable(true);
                                button->setChecked(true);
                        }

                        QObejct::connect(button, &QToolButton::clicked, this, [this, idx]() {
                                setCurrentIndex(idx);
                                emit activated(idx);
                        });

                        m_containerLayout->addWidget(button);
                }

        // 3. Left arrow button
        auto* rightButton = new QToolButton(m_container);
        rightButton->setArrowType(Qt::RightArrow);
        rightButton->setEnabled(activeRow < totalRows - 1);
        QObject::connect(rightButton,
                         &QToolButton::clicked,
                         this,
                         [this, activeRow, totalRows, parentIdx]() {
                                 if (activeRow < totalRows - 1) {
                                         auto nextIndex = m_model->index(activeRow + 1, 0, parentIdx);
                                         setCurrentIndex(nextIndex);
                                         emit activated(nextIndex);
                                 }
                                 emit rightButtonClicked();
                         });
        m_containerLayout->addWidget(rightButton);

        m_containerLayout->addStretch(1);
}

void NfBreadcrumbView::onModelDataChanged([[maybe_unused]] const QModelIndex& topLeft,
                                          [[maybe_unused]] const QModelIndex& bottomRight)
{
        rebuildLayout();
}

void NfBreadcrumbView::onModelReset()
{
        rebuildLayout();
}

} // namespace NfDesktop
