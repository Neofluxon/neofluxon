/**
 * File name: NfSupportedFormats.h
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef NF_SUPPORTED_FORMATS_H
#define NF_SUPPORTED_FORMATS_H

#include "NfPhoto.h"

#include <string_view>
#include <unordered_map>

namespace NfCore {

namespace NfSupportedFormats {

inline const std::unordered_map<std::string_view, NfPhoto::PhotoFormat> all{
        {".cr3", NfPhoto::PhotoFormat::Raw},
        {".cr2", NfPhoto::PhotoFormat::Raw},
        {".dng", NfPhoto::PhotoFormat::Raw},
        {".raf", NfPhoto::PhotoFormat::Raw},
        {".nef", NfPhoto::PhotoFormat::Raw},
        {".jpg", NfPhoto::PhotoFormat::Jpeg},
        {".jpeg", NfPhoto::PhotoFormat::Jpeg},
        {".png", NfPhoto::PhotoFormat::Png},
};

} // namespace NfSupportedFormats

} // namespace NfCore

#endif // NF_SUPPORTED_FORMATS_H
