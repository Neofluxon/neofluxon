/**
 * File name: NfExifMetadataExtractor.cpp
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

#include "NfExifMetadataExtractor.h"
#include "NfLogger.h"

#include <exiv2/exiv2.hpp>

#include <charconv>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <initializer_list>
#include <sstream>

namespace NfCore {

namespace {

const char* formatName(NfPhoto::PhotoFormat format)
{
        switch (format) {
        case NfPhoto::PhotoFormat::Jpeg:
                return "JPEG";
        case NfPhoto::PhotoFormat::Png:
                return "PNG";
        default:
                return "unknown";
        }
}

std::string exifString(const Exiv2::ExifData& data, const char* key)
{
        const auto iterator = data.findKey(Exiv2::ExifKey(key));
        return iterator == data.end() ? std::string{} : iterator->toString();
}

std::string xmpString(const Exiv2::XmpData& data, const char* key)
{
        const auto iterator = data.findKey(Exiv2::XmpKey(key));
        return iterator == data.end() ? std::string{} : iterator->toString();
}

std::string iptcString(const Exiv2::IptcData& data, const char* key)
{
        const auto iterator = data.findKey(Exiv2::IptcKey(key));
        return iterator == data.end() ? std::string{} : iterator->toString();
}

double exifNumber(const Exiv2::ExifData& data, const char* key)
{
        const auto iterator = data.findKey(Exiv2::ExifKey(key));
        return iterator == data.end() ? 0.0 : iterator->toFloat();
}

int exifInteger(const Exiv2::ExifData& data, const char* key, int fallback = 0)
{
        const auto iterator = data.findKey(Exiv2::ExifKey(key));
        return iterator == data.end() ? fallback : static_cast<int>(iterator->toInt64());
}

int parseInteger(const std::string& value, int fallback = 0)
{
        int result = fallback;
        const auto* first = value.data();
        const auto* last = first + value.size();
        const auto conversion = std::from_chars(first, last, result);
        return conversion.ec == std::errc{} && conversion.ptr == last ? result : fallback;
}

std::chrono::system_clock::time_point parseExifDate(const std::string& value)
{
        std::tm date{};
        std::istringstream stream(value);
        stream >> std::get_time(&date, "%Y:%m:%d %H:%M:%S");
        if (stream.fail())
                return {};

        return std::chrono::system_clock::from_time_t(timegm(&date));
}

double gpsCoordinate(const Exiv2::ExifData& data,
                     const char* key,
                     const char* referenceKey)
{
        const auto iterator = data.findKey(Exiv2::ExifKey(key));
        if (iterator == data.end() || iterator->count() < 3)
                return 0.0;

        auto coordinate = static_cast<double>(iterator->toFloat(0))
                + static_cast<double>(iterator->toFloat(1)) / 60.0
                + static_cast<double>(iterator->toFloat(2)) / 3600.0;
        const auto reference = exifString(data, referenceKey);
        if (reference == "S" || reference == "W")
                coordinate = -coordinate;
        return coordinate;
}

std::string firstNonEmpty(std::initializer_list<std::string> values)
{
        for (const auto& value : values) {
                if (!value.empty())
                        return value;
        }
        return {};
}

} // namespace

NfExifMetadataExtractor::NfExifMetadataExtractor(const NfPhoto& photo)
        : m_photo{photo}
{
}

std::optional<NfPhotoMetadata> NfExifMetadataExtractor::metadata() const
{
        const auto& path = m_photo.path();
        const auto format = formatName(m_photo.format());
        NF_LOG_DEBUG("read " << format << " metadata: " << path);

        try {
                auto image = Exiv2::ImageFactory::open(path.string());
                if (!image) {
                        NF_LOG_ERROR("can't open image with Exiv2: " << path);
                        return std::nullopt;
                }

                image->readMetadata();
                const auto& exif = image->exifData();
                const auto& xmp = image->xmpData();
                const auto& iptc = image->iptcData();

                NfPhotoMetadata metadata;
                metadata.width = static_cast<int>(image->pixelWidth());
                metadata.height = static_cast<int>(image->pixelHeight());
                metadata.format = format;

                std::error_code error;
                const auto fileSize = std::filesystem::file_size(path, error);
                if (!error)
                        metadata.fileSize = static_cast<std::int64_t>(fileSize);

                metadata.cameraMaker = exifString(exif, "Exif.Image.Make");
                metadata.cameraModel = exifString(exif, "Exif.Image.Model");
                const auto lensName = Exiv2::lensName(exif);
                metadata.lens = lensName == exif.end()
                        ? firstNonEmpty({
                                  exifString(exif, "Exif.Photo.LensModel"),
                                  exifString(exif, "Exif.Photo.LensMake")
                          })
                        : lensName->toString();

                metadata.iso = exifInteger(exif, "Exif.Photo.ISOSpeedRatings", -1);
                metadata.aperture = exifNumber(exif, "Exif.Photo.FNumber");
                metadata.shutterSpeed = exifNumber(exif, "Exif.Photo.ExposureTime");
                metadata.focalLength = exifNumber(exif, "Exif.Photo.FocalLength");
                metadata.dateTaken = parseExifDate(firstNonEmpty({
                        exifString(exif, "Exif.Photo.DateTimeOriginal"),
                        exifString(exif, "Exif.Photo.DateTimeDigitized"),
                        exifString(exif, "Exif.Image.DateTime")
                }));

                metadata.description = firstNonEmpty({
                        exifString(exif, "Exif.Image.ImageDescription"),
                        xmpString(xmp, "Xmp.dc.description"),
                        iptcString(iptc, "Iptc.Application2.Caption")
                });
                metadata.title = firstNonEmpty({
                        xmpString(xmp, "Xmp.dc.title"),
                        iptcString(iptc, "Iptc.Application2.Headline")
                });
                metadata.author = firstNonEmpty({
                        exifString(exif, "Exif.Image.Artist"),
                        xmpString(xmp, "Xmp.dc.creator"),
                        iptcString(iptc, "Iptc.Application2.Byline")
                });
                metadata.license = firstNonEmpty({
                        exifString(exif, "Exif.Image.Copyright"),
                        xmpString(xmp, "Xmp.dc.rights")
                });
                metadata.rating = exifInteger(exif, "Exif.Image.Rating");
                if (metadata.rating == 0)
                        metadata.rating = parseInteger(xmpString(xmp, "Xmp.xmp.Rating"));
                metadata.tags = firstNonEmpty({
                        xmpString(xmp, "Xmp.dc.subject"),
                        iptcString(iptc, "Iptc.Application2.Keywords")
                });

                metadata.bitDepth = exifInteger(exif, "Exif.Image.BitsPerSample");
                const auto colorSpace = exifInteger(exif, "Exif.Photo.ColorSpace");
                metadata.colorSpace = colorSpace == 1
                        ? "sRGB"
                        : colorSpace == 2
                        ? "Adobe RGB"
                        : "";
                metadata.orientation = exifInteger(exif, "Exif.Image.Orientation", 1);

                const auto latitudeRef = exifString(exif, "Exif.GPSInfo.GPSLatitudeRef");
                const auto longitudeRef = exifString(exif, "Exif.GPSInfo.GPSLongitudeRef");
                if (!latitudeRef.empty() && !longitudeRef.empty()
                    && exif.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude")) != exif.end()
                    && exif.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude")) != exif.end()) {
                        metadata.latitude = gpsCoordinate(
                                exif, "Exif.GPSInfo.GPSLatitude", "Exif.GPSInfo.GPSLatitudeRef");
                        metadata.longitude = gpsCoordinate(
                                exif, "Exif.GPSInfo.GPSLongitude", "Exif.GPSInfo.GPSLongitudeRef");
                        metadata.altitude = exifNumber(exif, "Exif.GPSInfo.GPSAltitude");
                        if (exifInteger(exif, "Exif.GPSInfo.GPSAltitudeRef") == 1)
                                metadata.altitude = -metadata.altitude;
                        metadata.hasGps = true;
                }

                metadata.hasExif = !exif.empty();
                return metadata;
        }
        catch (const Exiv2::Error& error) {
                NF_LOG_ERROR("can't read " << format << " metadata from " << path
                             << ": " << error.what());
                return std::nullopt;
        }
        catch (const std::exception& error) {
                NF_LOG_ERROR("can't parse " << format << " metadata from " << path
                             << ": " << error.what());
                return std::nullopt;
        }
}

} // namespace NfCore
