/**
 * File name: NfQtDateTimeUtils.h
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

#ifndef NF_QT_DAETIME_UTILS_H
#define NF_QT_DAETIME_UTILS_H

#include <QDateTime>
#include <QString>

#include <chrono>

class QLocale;

namespace NfUi {

class NfQtDateTimeUtils
{
public:
        NfQtDateTimeUtils() = delete;

        static QDateTime toQDateTime(const std::chrono::system_clock::time_point& timePoint);
        static std::chrono::system_clock::time_point toTimePoint(const QDateTime& dateTime);
        static QString formatDate(const std::chrono::system_clock::time_point& timePoint,
                                  const QString& format,
                                  const QLocale& locale = QLocale());
};

} // namespace NfUi

#endif // NF_QT_PATH_UTILS_H
