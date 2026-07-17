/**
 * File name: NfLibraryTreeItem.cpp
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

#include "NfLibraryTreeItem.h"
#include "core/NfLogger.h"

using namespace NfCore;

namespace NfDesktop {

NfLibraryTreeItem::NfLibraryTreeItem(const QString& name,
                                     NodeType type,
                                     NfLibraryTreeItem* parent)
        : m_name{name}
        , m_type{type}
        , m_parent{parent}
{
}

NfLibraryTreeItem::~NfLibraryTreeItem() = default;

void NfLibraryTreeItem::appendChild(std::unique_ptr<NfLibraryTreeItem> child)
{
        m_children.push_back(std::move(child));
}

NfLibraryTreeItem* NfLibraryTreeItem::child(int row) const
{
        if (row < 0 || row >= static_cast<int>(m_children.size()))
                return nullptr;

        return m_children[row].get();
}

NfLibraryTreeItem* NfLibraryTreeItem::parent() const
{
        return m_parent;
}

int NfLibraryTreeItem::childCount() const
{
        return static_cast<int>(m_children.size());
}

int NfLibraryTreeItem::row() const
{
        if (!m_parent)
                return 0;

        for (size_t i = 0; i < m_parent->m_children.size(); i++) {
                if (m_parent->m_children[i].get() == this)
                        return static_cast<int>(i);
        }

        return 0;
}

QString NfLibraryTreeItem::name() const
{
        return m_name;
}

NfLibraryTreeItem::NodeType NfLibraryTreeItem::type() const
{
        return m_type;
}

void NfLibraryTreeItem::setValue(const NfLibraryTreeItem::Value &v)
{
        m_value = v;
}

NfLibraryTreeItem::Value NfLibraryTreeItem::value() const
{
        return m_value;
}

NfLibraryQuery NfLibraryTreeItem::makeQuery() const
{
        NfLibraryQuery query;
        query.queryValue = value();

        for (const NfLibraryTreeItem* node = this; node; node = node->parent()) {
                switch (node->type()) {
                case NodeType::Library:
                        query.libraryId = std::get<int64_t>(node->value());
                        return query;

                case NodeType::Representation:
                        if (const auto value = node->value();
                            const auto* rep = std::get_if<int>(&value)) {
                                query.representationType =
                                        static_cast<RepresentationType>(*rep);
                        }
                        break;

                default:
                        break;
                }
        }

        return query;
}

} // namespace NfDesktop
