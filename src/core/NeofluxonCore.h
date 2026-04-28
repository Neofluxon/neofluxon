/**
 * File name: NeofluxonCore.h
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

#ifndef NEOFLUXON_CORE_H
#define NEOFLUXON_CORE_H

#include <memory>

namespace NfCore {

class NfPhotoLoader;
class NfCache;

class NeofluxonCore {
public:
        NeofluxonCore();
        ~NeofluxonCore();
        NeofluxonCore(const NeofluxonCore&) = delete;
        NeofluxonCore& operator=(const NeofluxonCore&) = delete;
        NeofluxonCore(NeofluxonCore&&) noexcept = default;
        NeofluxonCore& operator=(NeofluxonCore&&) noexcept = default;

        NfPhotoLoader* photoLoader() const;
        NfCache* thumbnailCache() const;
        NfCache* previewCache() const;

private:
        std::unique_ptr<NfPhotoLoader> m_photoLoader;
        std::unique_ptr<NfCache> m_thumbnailCache;
        std::unique_ptr<NfCache> m_previewCache;
};

} // namespace NfCore

#endif // NEOFLUXON_CORE_H
