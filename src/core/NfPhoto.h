/**
 * File name: NfPhoto.h
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

#ifndef NF_PHOTO_H
#define NF_PHOTO_H

#include "NfPhotoId.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace NfCore {

class NfPhoto {
 public:
        enum class PhotoFormat {
                Unknown,
                Jpeg,
                Raw,
                Png
        };

        explicit NfPhoto(const std::filesystem::path &filePath = {});
        const std::filesystem::path& path() const;
        const NfPhotoId& id() const;
        void setName(std::string_view name);
        const std::string& name() const;
        void setSizeOnDisk(std::uintmax_t size);
        std::uintmax_t sizeOnDisk() const;
        void setFormat(PhotoFormat format);
        NfPhoto::PhotoFormat format() const;
        void setDateTaken(const std::chrono::system_clock::time_point &date);
        const std::chrono::system_clock::time_point& dateTaken() const;
        void setCameraMaker(std::string_view maker);
        const std::string& cameraMaker() const;
        void setCameraModel(std::string_view model);
        const std::string& cameraModel() const;
        void setLens(std::string_view lens);
        const std::string& lens() const;
        bool isValid() const;
        bool loadSummary(bool fileSystemOnly = false);

 private:
        PhotoFormat determineFormat(const std::filesystem::path& path);

        std::filesystem::path m_filePath;
        NfPhotoId m_photoId;
        std::string m_name;
        std::uintmax_t m_sizeOnDisk{0};
        PhotoFormat m_format{};
        std::chrono::system_clock::time_point m_dateTaken{};
        std::string m_cameraMaker;
        std::string m_cameraModel;
        std::string m_lens;
};

} // namespace NfCore

#endif // NF_PHOTO_H
