/**
 * File name: NfThreadAllocationManager.h
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

#ifndef NF_THREAD_ALLOCATION_MANAGER_H
#define NF_THREAD_ALLOCATION_MANAGER_H

#pragma once

namespace NfCore {

class NfThreadAllocationManager {
 public:
        explicit NfThreadAllocationManager(unsigned int overrideHardwareConcurrency = 0) noexcept;
        NfThreadAllocationManager(const NfThreadAllocationManager&) = delete;
        NfThreadAllocationManager& operator=(const NfThreadAllocationManager&) = delete;
        NfThreadAllocationManager(NfThreadAllocationManager&&) noexcept = default;
        NfThreadAllocationManager& operator=(NfThreadAllocationManager&&) noexcept = default;
        ~NfThreadAllocationManager() = default;
        [[nodiscard]] unsigned int getTotalThreads() const noexcept;
        [[nodiscard]] unsigned int getForegroundThreadCount() const noexcept;
        [[nodiscard]] unsigned int getBackgroundThreadCount() const noexcept;

 private:
        void calculateAllocation() noexcept;

 private:
        unsigned int m_totalAvailableThreads{0};
        unsigned int m_foregroundThreads{0};
        unsigned int m_backgroundThreads{0};
};

} // namespace NfCore

#endif // NF_THREAD_ALLOCATION_MANAGER_H
