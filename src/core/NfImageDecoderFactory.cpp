/**
 * File name: NfImageDecoderFactory.cpp
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

#include "NfImageDecoderFactory.h"
#include "NfPhoto.h"
#include "NfJpegImageDecoder.h"
#include "NfPngImageDecoder.h"
#include "NfRawImageDecoder.h"

namespace NfCore {

std::unique_ptr<NfImageDecoder> NfImageDecoderFactory::createDecoder(const NfPhoto& photo)
{
        switch (photo.format()) {
        case NfPhoto::PhotoFormat::Jpeg:
                return std::make_unique<NfJpegImageDecoder>(photo);
        case NfPhoto::PhotoFormat::Raw:
                return std::make_unique<NfRawImageDecoder>(photo);
        case NfPhoto::PhotoFormat::Png:
                return std::make_unique<NfPngImageDecoder>(photo);
                break;
        case NfPhoto::PhotoFormat::Unknown:
        default:
                return nullptr;
        }
}
} // namespace NfCore
