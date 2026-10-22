/**
 * File name: NfScopedTimer.h
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

#ifndef NF_SCOPED_TIMER_H
#define NF_SCOPED_TIMER_H

#include "NfLogger.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <utility>

namespace NfCore {

class NfScopedTimer {
public:
        NfScopedTimer(std::string label, std::filesystem::path path)
                : m_label{std::move(label)}
                , m_path{std::move(path)}
                , m_start{std::chrono::steady_clock::now()}
        {
        }

        ~NfScopedTimer()
        {
                const auto elapsed = std::chrono::duration_cast<
                        std::chrono::microseconds>(
                                std::chrono::steady_clock::now() - m_start);
                NF_LOG_INFO(m_label << " [" << m_path
                                    << "] execution time: " << elapsed.count() << " us");
        }

private:
        std::string m_label;
        std::filesystem::path m_path;
        std::chrono::steady_clock::time_point m_start;
};

} // namespace NfCore

#endif // NF_SCOPED_TIMER_H
