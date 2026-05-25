/**
 * File name: NfLibraryRepresentation.cpp
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

#include "NfLibraryRepresentation.h"
#include "NfLibraryDatabase.h"
//#include "NfLibraryTreeNode.h"

namespace NfCore {

NfLibraryRepresentation::NfLibraryRepresentation(NfLibraryDatabase *db, uint64_t id)
//: m_database{db}
          //, m_id{id}
{
        //auto rec = db->getRepresentationRecord(id);
        //m_name = rec->name;
        //loadTree(rec->representationTreeRecord);
}

NfLibraryRepresentation::~NfLibraryRepresentation()
{
}

void NfLibraryRepresentation::setName(const std::string& name)
{
        m_name = name;
}

const std::string& NfLibraryRepresentation::name() const
{
        return m_name;
}

        /*NfLibraryTreeNode* NfLibraryRepresentation::getTree() const
{
        return m_tree.get();
        }*/

} // namespace NfCore
