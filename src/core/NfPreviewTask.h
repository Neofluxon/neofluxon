/**
 * File name: NfPreviewTask.h
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

#ifndef NF_PREVIEW_TASK_H
#define NF_PREVIEW_TASK_H

#include "NfImageTask.h"
#include "NfPhoto.h"
#include "NfPreview.h"

#include <memory>

namespace NfCore {

class NfImage;

class NfPreviewTask : public NfImageTask {
public:
        NfPreviewTask(const NfPhoto& photo);
        NfPreviewTask(NfPreviewTask&&) noexcept = default;
        NfPreviewTask& operator=(NfPreviewTask&&) noexcept = default;
        NfPreviewTask(const NfPreviewTask&) = delete;
        NfPreviewTask& operator=(const NfPreviewTask&) = delete;
        ~NfPreviewTask();

        TaskStatus execute() override;
        std::unique_ptr<NfPreview> takePreview();
};

} // namespace NfCore

#endif // NF_PREVIEW_TASK_H
