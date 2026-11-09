/**
 * File name: Neofluxon.h
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

#ifndef NEOFLUXON_H
#define NEOFLUXON_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <utility>

#define NEOFLUXON_VERSION NEOFLUXON_VERSION_HEX
#define NEOFLUXON_VERSION_MAJOR ((NEOFLUXON_VERSION_HEX >> 16) & 0xFF)
#define NEOFLUXON_VERSION_MINOR ((NEOFLUXON_VERSION_HEX >> 8) & 0xFF)
#define NEOFLUXON_VERSION_PATCH (NEOFLUXON_VERSION_HEX & 0xFF)
#define NEOFLUXON_VERSION_STRING NEOFLUXON_VERSION_STR

#if defined(_WIN32) || defined(_WIN64)
    #define NF_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define NF_PLATFORM_GNU
#else
    #error "Neofluxon: Platform not implemented"
#endif

#endif // NEOFLUXON_H
