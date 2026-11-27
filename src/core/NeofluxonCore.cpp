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
#include "NfApplicationPaths.h"
#include "NfDatabase.h"
#include "NfProjectsManager.h"
#include "NfLibraryManager.h"
#include "NfCache.h"
#include "NfDiskCache.h"
#include "NfSettings.h"
#include "NfScheduler.h"
#include "NfThreadPool.h"
#include "NfUserTaskManager.h"
#include "NfLogger.h"
#include "utils/NfFileUtils.h"

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
        m_foregroundScheduler->cancelAll();
        m_backgroundScheduler->cancelAll();
        m_foregroundThreadPool.reset();
        m_backgroundThreadPool.reset();
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
                                + NfFileUtils::pathToUtf8(directory) + "': "
                                + (error ? error.message() : "path is not a directory");
                        NF_LOG_ERROR(errorMessage);
                        return false;
                }

                NF_LOG_DEBUG("path created: " << directory);
        }

        try {
                std::string settingsError;
                auto setting = NfSettings::load(&settingsError);
                if (!settingsError.empty())
                        NF_LOG_ERROR(settingsError);
                m_settings = setting;

                auto threadAllocationManager = std::make_unique<NfThreadAllocationManager>();

                auto diskCache = std::make_unique<NfDiskCache>(
                        setting.diskCacheDirectory,
                        setting.diskCacheMaxSizeBytes,
                        setting.minimumFreeDiskSpaceBytes);

                const auto thumbnailMemory = setting.thumbnailMemoryAllocation();
                const auto previewMemory = setting.previewMemoryAllocation();
                auto thumbnailCache = std::make_unique<NfCache>(
                        static_cast<std::size_t>(thumbnailMemory.decodedBytes),
                        diskCache.get(),
                        NfDiskCache::CacheType::Thumbnail);
                auto previewCache = std::make_unique<NfCache>(
                        static_cast<std::size_t>(previewMemory.decodedBytes),
                        diskCache.get(),
                        NfDiskCache::CacheType::Preview);

                auto foregroundScheduler = std::make_unique<NfScheduler>();
                auto foregroundThreadPool = std::make_unique<NfThreadPool>(foregroundScheduler.get(),
                                                                           threadAllocationManager->getForegroundThreadCount());
                auto backgroundScheduler = std::make_unique<NfScheduler>();
                auto backgroundThreadPool = std::make_unique<NfThreadPool>(backgroundScheduler.get(),
                                                                           threadAllocationManager->getBackgroundThreadCount());

                auto userTaskManager = std::make_unique<NfUserTaskManager>();

                auto dbPath = NfApplicationPaths().dataDirectory() / "neofluxon.db";
                auto neofluxonDatabase = std::make_unique<NfDatabase>(dbPath);
                if (!neofluxonDatabase->initialize()) {
                        NF_LOG_ERROR("can't create database");
                        return false;
                }

                auto projectsManager = std::make_unique<NfProjectsManager>(neofluxonDatabase.get());
                auto libraryManager = std::make_unique<NfLibraryManager>(neofluxonDatabase.get(),
                                                                         backgroundScheduler.get(),
                                                                         foregroundScheduler.get(),
                                                                         userTaskManager.get());

                auto photoLoader = std::make_unique<NfPhotoLoader>(NfPhotoLoaderContext{
                        .thumbnailCache = thumbnailCache.get(),
                        .previewCache = previewCache.get(),
                        .diskCache = diskCache.get(),
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
                m_diskCache = std::move(diskCache);
                m_foregroundScheduler = std::move(foregroundScheduler);
                m_foregroundThreadPool = std::move(foregroundThreadPool);
                m_backgroundScheduler = std::move(backgroundScheduler);
                m_backgroundThreadPool = std::move(backgroundThreadPool);
                m_database = std::move(neofluxonDatabase);
                m_projectsManager = std::move(projectsManager);
                m_libraryManager = std::move(libraryManager);
                m_photoLoader = std::move(photoLoader);
                m_metadataLoader = std::move(metadataLoader);
                m_userTaskManager = std::move(userTaskManager);
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

NfProjectsManager* NeofluxonCore::projectsManager() const
{
        return m_projectsManager.get();
}

NfLibraryManager* NeofluxonCore::libraryManager() const
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

NfUserTaskManager* NeofluxonCore::userTaskManager() const
{
        return m_userTaskManager.get();
}

const NfSettings& NeofluxonCore::settings() const
{
        return m_settings;
}

} // namespace NfCore
