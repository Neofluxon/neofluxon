/**
 * File name: NfPreview.h
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

#ifndef NF_PREVIEW_H
#define NF_PREVIEW_H

#include "NfPhotoId.h"

#include <memory>

namespace NfCore {

class NfImage;

class NfPreview {
public:
        explicit NfPreview(const NfPhotoId &id, std::unique_ptr<NfImage> img);
        NfPreview(NfPreview&&) noexcept = default;
        NfPreview& operator=(NfPreview&&) noexcept = default;
        NfPreview(const NfPreview&) = delete;
        NfPreview& operator=(const NfPreview&) = delete;

        const NfPhotoId& id() const;
        NfImage* getImage() const;
        std::unique_ptr<NfImage> releaseImage();

private:
        NfPhotoId m_photoId;
        std::unique_ptr<NfImage> m_image;
};

} // namespace NfCore

#endif // NF_PERVIEW_H
