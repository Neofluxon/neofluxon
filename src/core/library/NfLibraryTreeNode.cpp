/**
 * File name: NfLibraryTreeNode.cpp
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

#include "NfLibraryTreeNode.h"

#include <algorithm>

namespace NfCore {

NfLibraryTreeNode::NfLibraryTreeNode()
{
}

NfLibraryTreeNode::NfLibraryTreeNode(const std::string& name)
        : m_name(name)
{
}

NfLibraryTreeNode::~NfLibraryTreeNode()
{
}

void NfLibraryTreeNode::setName(const std::string& name)
{
        m_name = name;
}

const std::string& NfLibraryTreeNode::type() const
{
        return m_type;
}

void NfLibraryTreeNode::setType(const std::string& t)
{
        m_type = t;
}

const std::string& NfLibraryTreeNode::name() const
{
        return m_name;
}

NfLibraryTreeNode* NfLibraryTreeNode::addChild()
{
        auto child = std::make_unique<NfLibraryTreeNode>();
        NfLibraryTreeNode* ptr = child.get();

        m_children.push_back(std::move(child));

        return ptr;
}

void NfLibraryTreeNode::removeChild(NfLibraryTreeNode* child)
{
        auto it = std::remove_if(m_children.begin(),
                                 m_children.end(),
                                 [child](const auto& item) {
                                         return item.get() == child;
                                 });

        m_children.erase(it, m_children.end());
}

const std::vector<std::unique_ptr<NfLibraryTreeNode>>&
NfLibraryTreeNode::children() const
{
        return m_children;
}

} // namespace NfCore
