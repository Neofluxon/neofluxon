/**
 * File name: NfImageTask.h
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

#ifndef NF_IAMGE_TASK_H
#define NF_IAMGE_TASK_H

#include "NfTask.h"
#include "NfPhoto.h"
#include "NfThumbnail.h"

#include <memory>
#include <cstdint>

namespace NfCore {

class NfImage;

class NfImageTask : public NfTask {
public:
        enum class ExtractionMethod {
                Embedded,
                FromRaw,

                // Attempt to extract the embedded image;
                // fallback to RAW decoding if unavailable.
                Fastest
        };

        NfImageTask(const NfPhoto& photo);
        NfImageTask(NfImageTask&&) noexcept = default;
        NfImageTask& operator=(NfImageTask&&) noexcept = default;
        NfImageTask(const NfImageTask&) = delete;
        NfImageTask& operator=(const NfImageTask&) = delete;
        ~NfImageTask();

        void setExtractionMethod(ExtractionMethod method);
        ExtractionMethod extractionMethod() const;
        NfImage* getImage() const;
        std::unique_ptr<NfImage> takeImage();
        const NfPhoto& getPhoto() const;

protected:
        NfImage* imageContainer() const;

private:
        NfPhoto m_photo;
        std::unique_ptr<NfImage> m_image;
        ExtractionMethod m_extractionMethod;
};

} // namespace NfCore

#endif // NF_IMAGE_TASK_H
