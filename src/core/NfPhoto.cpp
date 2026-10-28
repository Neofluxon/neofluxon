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

namespace NfCore {

        NfPhoto::NfPhoto(const std::filesystem::path &filePath, bool loadInfo)
        : m_filePath{filePath}
        , m_photoId{filePath}
        , m_name{NfFileUtils::pathToUtf8(m_filePath.filename())}
{
        if (m_photoId.isValid() && loadInfo) {
                m_fileEixsts = NfFileUtils::fileExists(m_filePath);
                if (m_fileEixsts) {
                        m_sizeOnDisk = NfFileUtils::fileSize(m_filePath);
                        NfPhotoMetadataExtractor extrator(this);
                        auto summary = extrator.summaryInfo();
                        if (summary) {
                                m_format = determineFormat(summary.format);
                                m_dateTaken = summary.dateTaken;
                        }
                }
        }
}

const NfPhotoId& NfPhoto::id() const
{
        return m_photoId;
}

const std::filesystem::path& NfPhoto::path() const
{
        return m_filePath;
}

std::string NfPhoto::name() const
{
        return m_name;
}

std::uintmax_t NfPhoto::sizeOnDisk() const
{
        return m_sizeOnDisk;
}


bool NfPhoto::fileExists() const
{
        return m_fileEixsts;
}

NfPhoto::PhotoFormat NfPhoto::format() const
{
        return m_format;
}

std::optional<std::chrono::system_clock::time_point> dateTaken() const
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

} // namespace NfCore
