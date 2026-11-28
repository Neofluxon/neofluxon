/**
 * File name: NfQtPathUtils.cpp
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

#include "NfQtDateTimeUtils.h"

#include <QLocale>

namespace NfUi {

QDateTime NfQtDateTimeUtils::toQDateTime(const std::chrono::system_clock::time_point& timePoint){
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                timePoint.time_since_epoch()).count();
        return QDateTime::fromMSecsSinceEpoch(ms);
}

std::chrono::system_clock::time_point
NfQtDateTimeUtils::toTimePoint(const QDateTime& dateTime)
{
        return std::chrono::system_clock::time_point{
                std::chrono::milliseconds{dateTime.toMSecsSinceEpoch()}};
}

QString NfQtDateTimeUtils::formatDate(const std::chrono::system_clock::time_point& timePoint,
                                    const QString& format,
                                    const QLocale& locale)
{
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                timePoint.time_since_epoch()).count();
        auto dateTime = QDateTime::fromMSecsSinceEpoch(ms);
        return locale.toString(dateTime, format);
}

} // namespace NfUi
