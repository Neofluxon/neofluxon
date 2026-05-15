/**
 * File name: NeofluxonCore.cpp
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

#include "NeofluxonCore.h"
#include "NfPhotoLoader.h"
#include "NfCache.h"
#include "NfScheduler.h"
#include "NfThreadPool.h"
#include "NfLogger.h"

namespace NfCore {

NeofluxonCore::NeofluxonCore()
        : m_thumbnailCache{std::make_unique<NfCache>(2 * NfCache::DEFAULT_MAX_SIZE_BYTES)}
        , m_previewCache{std::make_unique<NfCache>()}
        , m_foregroundScheduler{std::make_unique<NfScheduler>()}
        , m_foregroundThreadPool{std::make_unique<NfThreadPool>(m_foregroundScheduler.get(),
                                                                m_threadAllocationManager.getForegroundThreadCount())}
        , m_backgroundScheduler{std::make_unique<NfScheduler>()}
        , m_backgroundThreadPool{std::make_unique<NfThreadPool>(m_backgroundScheduler.get(),
                                                                m_threadAllocationManager.getBackgroundThreadCount())}
        , m_photoLoader{std::make_unique<NfPhotoLoader>(m_thumbnailCache.get(),
                                                        m_previewCache.get()),
                                                        m_foregroundScheduler.get()}
{
        NF_LOG_DEBUG("called");
}

NeofluxonCore::~NeofluxonCore()
{
        NF_LOG_DEBUG("called");
}

NfPhotoLoader* NeofluxonCore::photoLoader() const
{
        return m_photoLoader.get();
}

NfCache* NeofluxonCore::thumbnailCache() const
{
        return m_thumbnailCache.get();
}

NfCache* NeofluxonCore::previewCache() const
{
        return m_previewCache.get();
}

} // namespace NfCore
