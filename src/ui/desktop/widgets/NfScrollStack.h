/**
 * File name: NfScrollStack.h
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

#ifndef NF_SCROLL_STACK_H
#define NF_SCROLL_STACK_H

#include <QScrollArea>

class QWidget;
class QVBoxLayout;

namespace NfDesktop {

class NfScrollStack : public QScrollArea {
        Q_OBJECT

 public:
        explicit NfScrollStack(QWidget* parent = nullptr);
        void addWidget(QWidget* widget);
        void insertWidget(int index, QWidget* widget);
        void addStretch(int stretch = 0);

private:
        QWidget* m_scrollContent;
        QVBoxLayout* m_mainLayout;
};

} // namespace NfDesktop {

#endif // NF_SCROLL_STACK_H
