/**
 * File name: NfReqeust.h
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

#ifndef NF_REQUEST_H
#define NF_REQUEST_H

#include "NfTask.h"

namespace NfCore {

// Defines application-wide request types.
enum class NfRequestType : int {
        // Active/ in focused items
        SelectedMetadata,
        SelectedPreview,

        // Render in the viewport
        VisibleThumbnail,

        // Anticipated to enter the viewport
        PrefetchThumbnail,
        PrefetchPreview,
        PrefetchMetadata
};

namespace NfRequestUtils {
constexpr NfTask::Priority getTaskPriority(NfRequestType type)
{
        switch (type) {
        case NfRequestType::SelectedMetadata:
        case NfRequestType::SelectedPreview:
                return NfTask::Priority::Critical;
        case NfRequestType::VisibleThumbnail:
                return NfTask::Priority::Immediate;
        case NfRequestType::PrefetchThumbnail:
        case NfRequestType::PrefetchPreview:
        case NfRequestType::PrefetchMetadata:
                return NfTask::Priority::High;
        default:
                return NfTask::Priority::Normal;
        }
}
} // namespace NfRequestUtils

} // namespace NfCore

#endif // NF_REQUEST_H
