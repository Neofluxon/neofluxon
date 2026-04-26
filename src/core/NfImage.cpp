/**
 * File name: NfImage.cpp
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

#include "NfImage.h"

namespace NfCore {

NfImage::NfImage()
        : m_data(std::make_unique<NfImageData>())
{
}

NfImage::NfImage(std::unique_ptr<NfImageData> data)
        : m_data{std::move(data)}
{
}

NfImage::~NfImage() = default;

void NfImage::setData(std::unique_ptr<NfImageData> data)
{
        m_data = std::move(data);
}

NfImageData* NfImage::getData()
{
    return m_data.get();
}

const NfImageData* NfImage::getData() const
{
    return m_data.get();
}

int NfImage::setWidth() const
{
    return m_data ? m_data->setWidth() : 0;
}

int NfImage::width() const
{
    return m_data ? m_data->width() : 0;
}

int NfImage::height() const
{
    return m_data ? m_data->height() : 0;
}

int NfImage::channels() const
{
        return m_data ? m_data->channels() : 0;
}

NfImage::ImageFormat NfImage::format() const
{
        return m_data->format();
}

bool NfImage::isValid() const
{
        return !m_data->empty();
}

size_t NfImage::size() const
{
        return m_data->size();
}

void NfImage::resize(int w, int h)
{
}

} // namespace NfCore
