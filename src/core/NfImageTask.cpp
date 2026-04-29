/**
 * File name: NfImageTask.cpp
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

#include "NfImageTask.h"
#include "NfImage.h"

namespace NfCore {

NfImageTask::NfImageTask(const NfPhoto& photo)
        : m_generationId{0}
        , m_photo{photo}
        , m_image{std::make_unique<NfImage>()}
        , m_extractionMethod{ExtractionMethod::Fastest}
{
}

NfImageTask::~NfImageTask() = default;

void NfImageTask::setGenerationId(uint64_t generationId)
{
        m_generationId = generationId;
}

uint64_t NfImageTask::generationId() const
{
        return m_generationId;
}

void NfImageTask::setExtractionMethod(NfImageTask::ExtractionMethod method)
{
        m_extractionMethod = method;
}

NfImageTask::ExtractionMethod NfImageTask::extractionMethod() const
{
        return m_extractionMethod;
}

NfImage* NfImageTask::getImage() const
{
        return m_image.get();
}

std::unique_ptr<NfImage> NfImageTask::takeImage()
{
        return std::move(m_image);
}

const NfPhoto& NfImageTask::getPhoto() const
{
        return m_photo;
}

} // namespace NfCore
