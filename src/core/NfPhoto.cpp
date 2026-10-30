/**
 * File name: NfPhoto.cpp
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

#include "NfPhoto.h"
#include "utils/NfFileUtils.h"
#include "NfPhotoMetadataExtractor.h"
#include "NfScopedTimer.h"

namespace NfCore {

NfPhoto::NfPhoto(const std::filesystem::path &filePath)
        : m_filePath{filePath}
        , m_photoId{filePath}
{
}

const NfPhotoId& NfPhoto::id() const
{
        return m_photoId;
}

const std::filesystem::path& NfPhoto::path() const
{
        return m_filePath;
}

void NfPhoto::setName(std::string_view name)
{
        m_name = name;
}

const std::string& NfPhoto::name() const
{
        return m_name;
}

void NfPhoto::setSizeOnDisk(std::uintmax_t size)
{
        m_sizeOnDisk = size;
}

std::uintmax_t NfPhoto::sizeOnDisk() const
{
        return m_sizeOnDisk;
}

void NfPhoto::setFormat(PhotoFormat format)
{
        m_format = format;
}

NfPhoto::PhotoFormat NfPhoto::format() const
{
        return m_format;
}

void NfPhoto::setDateTaken(const std::chrono::system_clock::time_point &date)
{
        m_dateTaken = date;
}

const std::chrono::system_clock::time_point& NfPhoto::dateTaken() const
{
        return m_dateTaken;
}

NfPhoto::PhotoFormat NfPhoto::determineFormat(const std::filesystem::path& path)
{
        auto ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".cr3"
            || ext == ".cr2"
            || ext == ".dng"
            || ext == ".raf"
            || ext == ".nef")
                return PhotoFormat::Raw;

        if (ext == ".jpg" || ext == ".jpeg")
                return PhotoFormat::Jpeg;
        if (ext == ".png")
                return PhotoFormat::Png;

        return PhotoFormat::Unknown;
}

void NfPhoto::setCameraMaker(std::string_view maker)
{
        m_cameraMaker = maker;
}

const std::string& NfPhoto::cameraMaker() const
{
        return m_cameraMaker;
}

void NfPhoto::setCameraModel(std::string_view model)
{
        m_cameraModel = model;
}

const std::string& NfPhoto::cameraModel() const
{
        return m_cameraModel;
}

void NfPhoto::setLens(std::string_view lens)
{
        m_lens = lens;
}

const std::string& NfPhoto::lens() const
{
        return m_lens;
}

bool NfPhoto::loadSummary(bool fileSystemOnly)
{
        NfScopedTimer timer{"NfPhoto::loadSummary: ", name()};
        if (m_filePath.empty()
            || !m_photoId.isValid()
            || !NfFileUtils::fileExists(m_filePath))
                return false;

        m_name = NfFileUtils::pathToUtf8(m_filePath.filename());
        if (m_name.empty())
                return false;

        m_sizeOnDisk = NfFileUtils::fileSize(m_filePath).value_or(0);
        if (m_sizeOnDisk == 0)
                return false;

        m_format = determineFormat(m_filePath);
        if (m_format == PhotoFormat::Unknown)
                return false;

        if (fileSystemOnly) {
                m_dateTaken = NfFileUtils::modificationTime(m_filePath).value_or(
                                         std::chrono::system_clock::time_point{}
                              );
         } else {
                NfPhotoMetadataExtractor extractor(*this);
                auto summary = extractor.summaryInfo();
                if (summary) {
                        m_dateTaken = summary->dateTaken;
                        m_cameraMaker = summary->cameraMaker;
                        m_cameraModel = summary->cameraModel;
                        m_lens = summary->lens;
                }
        }

        return true;
}

bool NfPhoto::isValid() const
{
        return id().isValid()
                && !m_name.empty()
                && m_sizeOnDisk != 0
                && m_format != PhotoFormat::Unknown;
}

} // namespace NfCore
