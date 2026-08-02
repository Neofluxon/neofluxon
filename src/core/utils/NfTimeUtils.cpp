/**
 * File name: NfTimeUtils.cpp
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

#include "NfTimeUtils.h"

namespace NfCore {

NfDateRange NfTimeUtils::getYearRange(std::chrono::year yearValue)
{
        const std::chrono::system_clock::time_point start_tp{
                std::chrono::sys_days{yearValue / std::chrono::January / 1}
        };
        const std::chrono::system_clock::time_point end_tp{std::chrono::sys_days{
                        (yearValue + std::chrono::years{1}) / std::chrono::January / 1}
        };

        return NfDateRange{
                start_tp.time_since_epoch().count(),
                end_tp.time_since_epoch().count()
        };
}

NfDateRange NfTimeUtils::getMonthRange(std::chrono::year_month ym)
{
        const std::chrono::system_clock::time_point start_tp{
                std::chrono::sys_days{ym / 1}
        };
        const std::chrono::system_clock::time_point end_tp{
                std::chrono::sys_days{(ym + std::chrono::months{1}) / 1}
        };

        return NfDateRange{
                start_tp.time_since_epoch().count(),
                end_tp.time_since_epoch().count()
        };
}

NfDateRange NfTimeUtils::getDayRange(std::chrono::year_month_day ymd)
{
        const std::chrono::system_clock::time_point start_tp{
                std::chrono::sys_days{ymd}
        };
        const std::chrono::system_clock::time_point end_tp{
                std::chrono::sys_days{std::chrono::sys_days{ymd} + std::chrono::days{1}}
        };

        return NfDateRange{
                start_tp.time_since_epoch().count(),
                end_tp.time_since_epoch().count()
        };
}

NfDateRange NfTimeUtils::getYearRange(const std::tm& tm)
{
        return getYearRange(std::chrono::year{tm.tm_year + 1900});
}

int64_t NfTimeUtils::fromFileTime(std::filesystem::file_time_type ftime)
{
        const auto sys_tp = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
        return sys_tp.time_since_epoch().count();
}

} // namespace NfCore
