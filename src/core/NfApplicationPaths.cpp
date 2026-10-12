/**
 * File name: NfApplicationPaths.cpp
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

#include "NfApplicationPaths.h"

#include <cstdlib>

namespace NfCore {

namespace {
std::filesystem::path environmentPath(const char* variable)
{
        const auto* value = std::getenv(variable);
        return value && *value ? std::filesystem::path{value} : std::filesystem::path{};
}

std::filesystem::path xdgPath(const char* variable,
                              const std::filesystem::path& fallback)
{
        const auto path = environmentPath(variable);
        return path.is_absolute() ? path : fallback;
}

} // namespace

NfApplicationPaths::NfApplicationPaths()
{
#if defined(_WIN32)
        const auto roaming = environmentPath("APPDATA");
        const auto local = environmentPath("LOCALAPPDATA");
        m_dataDirectory = roaming / "Neofluxon";
        m_configDirectory = roaming / "Neofluxon";
        m_cacheDirectory = local / "Neofluxon" / "Cache";
#elif defined(__APPLE__)
        const auto home = environmentPath("HOME");
        const auto support = home / "Library" / "Application Support" / "Neofluxon";
        m_dataDirectory = support;
        m_configDirectory = support;
        m_cacheDirectory = home / "Library" / "Caches" / "Neofluxon";
#else
        const auto home = environmentPath("HOME");
        const auto appDirectory = std::filesystem::path{"neofluxon"};
        m_dataDirectory = xdgPath("XDG_DATA_HOME", home / ".local" / "share") / appDirectory;
        m_configDirectory = xdgPath("XDG_CONFIG_HOME", home / ".config") / appDirectory;
        m_cacheDirectory = xdgPath("XDG_CACHE_HOME", home / ".cache") / appDirectory;
#endif
}

std::filesystem::path NfApplicationPaths::dataDirectory() const
{
        return m_dataDirectory;
}

std::filesystem::path NfApplicationPaths::configDirectory() const
{
        return m_configDirectory;
}

std::filesystem::path NfApplicationPaths::cacheDirectory() const
{
        return m_cacheDirectory;
}

} // namespace NfCore
