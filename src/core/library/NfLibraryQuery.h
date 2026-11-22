/**
 * File name: NfLibraryQuery.h
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

#ifndef NF_LIBRARY_QUERY_H
#define NF_LIBRARY_QUERY_H

#include "NfLibraryRepresentation.h"
#include "NfTimeUtils.h"

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace NfCore {

struct NfEquipmentValue {
        enum class Kind {
                Camera,
                Lens
        };

        Kind kind;
        int64_t id;
        bool operator==(const NfEquipmentQuery&) const = default;
};

using NfQueryValue = std::variant<std::monostate,
                                  int,
                                  int64_t,
                                  std::string,
                                  NfDateRange,
                                  NfEquipmentValue>;

enum class NfLibrarySearchField {
        DateTaken,
        FileFormat,
        Folder,
        Camera,
        Lens,
};

enum class NfLibrarySearchOperator {
        Is,
        Contains,
        Before,
        After,
        Between
};

struct NfLibraryQueryCriterion {
        NfLibrarySearchField field;
        NfLibrarySearchOperator op;
        NfQueryValue value;
        bool operator==(const NfLibrarySearchCriterion&) const = default;
};

struct NfLibraryQuery {
        int64_t libraryId{-1};
        NfLibraryRepresentation::RepresentationType representationType{};
        NfQueryValue queryValue;
        std::vector<NfLibraryQueryCriterion> criteria;
        bool operator==(const NfLibraryQuery&) const = default;
};

} // namespace NfCore

#endif // NF_LIBRARY_QUERY_H
