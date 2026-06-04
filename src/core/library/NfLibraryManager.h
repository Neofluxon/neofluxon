/**
 * File name: NfLibraryManager.h
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

#ifndef NF_LIBRARY_MANAGER_H
#define NF_LIBRARY_MANAGER_H

#include <vector>
#include <memory>
#include <filesystem>
#include <mutex>
#include <string_view>

namespace NfCore {

class NfLibrary;
class NfScheduler;
class NfLibraryDatabase;

class NfLibraryManager {
public:
        NfLibraryManager(NfScheduler *scheduler);
        ~NfLibraryManager();
        const std::vector<std::unique_ptr<NfLibrary>>& libraries() const;
        void importPath(const std::filesystem::path& path, uint64_t id);
        NfLibrary* addLibrary(std::string_view name);
        NfLibrary* getLibrary(uint64_t id) const;
        NfLibrary* getLibraryByName(std::string_view name) const;

private:
        NfScheduler* m_scheduler;
        std::unique_ptr<NfLibraryDatabase> m_database;
        std::mutex m_mutex;
        std::vector<std::unique_ptr<NfLibrary>> m_libraries;
};

#endif // NF_LIBRARY_MANAGER_H

} // namespace NfCore
