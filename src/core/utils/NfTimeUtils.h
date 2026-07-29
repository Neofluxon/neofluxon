/**
 * File name: NfTimeUtils.h
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

#ifndef NF_TIME_UTILS_H
#define NF_TIME_UTILS_H

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <ctime>

namespace NfCore {

struct NfDateRange {
        int64_t start_ticks{0};
        int64_t end_ticks{0};
};

class NfTimeUtils {
 public:
        NfTimeUtils() = delete;
        static NfDateRange getYearRange(std::chrono::year yearValue);
        static NfDateRange getMonthRange(std::chrono::year_month ym);
        static NfDateRange getDayRange(std::chrono::year_month_day ymd);
        static int64_t fromFileTime(std::filesystem::file_time_type ftime);

        // Legacy std::tm overload (e.g. for EXIF or legacy C APIs)
        static NfDateRange getYearRange(const std::tm& tm);
};

} // NfCore

#endif // NF_TIME_UTILS_H
