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
#include "NfThreadAllocationManager.h"
#include "NfPhotoLoaderContext.h"
#include "NfPhotoLoader.h"
#include "NfPhotoMetadataLoader.h"
#include "NfLibraryManager.h"
#include "NfCache.h"
#include "NfScheduler.h"
#include "NfThreadPool.h"
#include "NfLogger.h"

#include <array>
#include <exception>
#include <filesystem>
#include <system_error>

namespace NfCore {

NeofluxonCore::NeofluxonCore()
{
        NF_LOG_DEBUG("called");
}

NeofluxonCore::~NeofluxonCore()
{
        NF_LOG_DEBUG("called");
}

bool NeofluxonCore::initialize(std::string& errorMessage)
{
        NF_LOG_DEBUG("called");

        NfApplicationPaths applicationPaths;
        const std::array<std::filesystem::path, 3> directories{
                applicationPaths.dataDirectory(),
                applicationPaths.configDirectory(),
                applicationPaths.cacheDirectory()
        };

        // Create application directories
        for (const auto& directory : directories) {
                std::error_code error;
                std::filesystem::create_directories(directory, error);
                if (error || !std::filesystem::is_directory(directory, error)) {
                        errorMessage = "Unable to create application directory '"
                                + directory.string() + "': "
                                + (error ? error.message() : "path is not a directory");
                        NF_LOG_ERROR(errorMessage);
                        return false;
                }

                NF_LOG_DEBUG("path created: " << directory);
        }

        try {
                auto threadAllocationManager = std::make_unique<NfThreadAllocationManager>();
                auto thumbnailCache = std::make_unique<NfCache>(2 * NfCache::DEFAULT_MAX_SIZE_BYTES);
                auto previewCache = std::make_unique<NfCache>();
                auto foregroundScheduler = std::make_unique<NfScheduler>();
                auto foregroundThreadPool = std::make_unique<NfThreadPool>(foregroundScheduler.get(),
                                                                           threadAllocationManager->getForegroundThreadCount());
                auto backgroundScheduler = std::make_unique<NfScheduler>();
                auto backgroundThreadPool = std::make_unique<NfThreadPool>(backgroundScheduler.get(),
                                                                           threadAllocationManager->getBackgroundThreadCount());

                auto libraryManager = std::make_unique<NfLibraryManager>(backgroundScheduler.get());
                if (!libraryManager->initialize(errorMessage)) {
                        NF_LOG_ERROR(errorMessage);
                        return false;
                }

                auto photoLoader = std::make_unique<NfPhotoLoader>(NfPhotoLoaderContext{
                        .thumbnailCache = thumbnailCache.get(),
                        .previewCache = previewCache.get(),
                        .scheduler = foregroundScheduler.get(),
                        .libraryManager = libraryManager.get()});

                auto metadataLoader = std::make_unique<NfPhotoMetadataLoader>(NfPhotoMetadataLoaderContext{
                        .thumbnailCache = thumbnailCache.get(),
                        .previewCache = previewCache.get(),
                        .scheduler = foregroundScheduler.get(),
                        .libraryManager = libraryManager.get()});

                m_threadAllocationManager = std::move(threadAllocationManager);
                m_thumbnailCache = std::move(thumbnailCache);
                m_previewCache = std::move(previewCache);
                m_foregroundScheduler = std::move(foregroundScheduler);
                m_foregroundThreadPool = std::move(foregroundThreadPool);
                m_backgroundScheduler = std::move(backgroundScheduler);
                m_backgroundThreadPool = std::move(backgroundThreadPool);
                m_libraryManager = std::move(libraryManager);
                m_photoLoader = std::move(photoLoader);
                m_metadataLoader = std::move(metadataLoader);
        } catch (const std::exception& exception) {
                errorMessage = "Unable to initialize application core: ";
                errorMessage += exception.what();

                NF_LOG_ERROR(errorMessage);

                return false;
        }

        errorMessage.clear();

        return true;
}

NfPhotoLoader* NeofluxonCore::photoLoader() const
{
        return m_photoLoader.get();
}

NfPhotoMetadataLoader* NeofluxonCore::metadataLoader() const
{
        return m_metadataLoader.get();
}

NfLibraryManager* NeofluxonCore::libraryManger() const
{
        return m_libraryManager.get();
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
