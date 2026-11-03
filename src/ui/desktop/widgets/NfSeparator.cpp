/**
 * File name: NfSeparator.cpp
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

#include "NfSeparator.h"

#include <algorithm>

namespace NfDesktop {

NfSeparator::NfSeparator(Orientation orientation, int thickness, QWidget* parent)
        : QFrame(parent)
        , m_orientation{orientation}
        , m_thickness{std::max(1, thickness)}
{
        setFrameShape(QFrame::NoFrame);
        setOrientation(m_orientation);
}

void NfSeparator::setOrientation(Orientation orientation)
{
        m_orientation = orientation;
        if (m_orientation == Orientation::Horizontal) {
                setFixedHeight(m_thickness);
                setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        } else {
                setFixedWidth(m_thickness);
                setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        }
}

NfSeparator::Orientation NfSeparator::orientation() const
{
        return m_orientation;
}

void NfSeparator::setThickness(int thickness)
{
        m_thickness = std::max(1, thickness);
        setOrientation(m_orientation);
}

int NfSeparator::thickness() const
{
        return m_thickness;
}

} // namespace NfDesktop
