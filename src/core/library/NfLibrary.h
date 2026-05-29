/**
 * File name: NfLibrary.h
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

#ifndef NF_LIBRARY_H
#define NF_LIBRARY_H

#include <string>
#include <vector>
#include <memory>
#include <string_view>

namespace NfCore {

class NfLibraryDatabase;
class NfLibraryRepresentation;
class NfPhoto;

class NfLibraryRepresentation;

class NfLibrary {
public:
        explicit NfLibrary(NfLibraryDatabase *db, uint64_t id);
        ~NfLibrary();

        uint64_t id() const;
        void setName(std::string_view& name);
        const std::string& name() const noexcept;

        NfLibraryRepresentation* addRepresentation();
        void removeRepresentation(NfLibraryRepresentation* representation);
        const std::vector<std::unique_ptr<NfLibraryRepresentation>>& representations() const noexcept;

        bool addPhoto(const NfPhoto& photo);

private:
        int storeCamera(const std::string& maker, const std::string& model);
        int storeLens(const std::string& lensName);

        NfLibraryDatabase *m_db;
        uint64_t m_id;
        std::string m_name;
        std::vector<std::unique_ptr<NfLibraryRepresentation>> m_representations;
};

} // namespace NfCore

#endif // NF_LIBRARY_H
