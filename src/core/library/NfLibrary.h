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

#include "NfPhoto.h"

#include <string>
#include <vector>
#include <memory>
#include <string_view>

namespace NfCore {

class NfLibraryDatabase;
class NfLibraryRepresentation;
class NfPhoto;
class NfLibraryQuery;

class NfLibraryRepresentation;

class NfLibrary {
public:
        explicit NfLibrary(NfLibraryDatabase *db, int64_t id);
        ~NfLibrary();

        int64_t id() const;
        void setName(std::string_view name);
        std::string name() const noexcept;
        NfLibraryRepresentation* addRepresentation();
        void removeRepresentation(NfLibraryRepresentation* representation);
        std::vector<std::unique_ptr<NfLibraryRepresentation>> representations() const;
        void addPhoto(const NfPhoto& photo);
        std::vector<NfPhoto> queryPhotos(const NfLibraryQuery &query) const;

private:
        int64_t storeCamera(std::string_view maker, std::string_view model);
        int64_t storeLens(std::string_view lensName);

        NfLibraryDatabase *m_db;
        int64_t m_id;
        std::string m_name;
        std::vector<std::unique_ptr<NfLibraryRepresentation>> m_representations;
};

} // namespace NfCore

#endif // NF_LIBRARY_H
