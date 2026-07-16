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
        const NfLibraryTreeItem* node = this;
        while (node) {
                if (node->type() == NodeType::Library) {
                        query.libraryId = std::get<int64_t>(node->value());
                        break;
                }

                if (node->type() == NodeType::Representation) {
                        if (const auto *nodeVal = std::get_if<int>(&query.queryValue))
                                query.representationType = static_cast<RepresentationType>(*nodeVal);
                }

                node = node->parent();
        }

        const auto v = value();
        if (const auto* val = std::get_if<int64_t>(&v)) {
                NF_LOG_DEBUG("node name: " << m_name.toStdString());
                NF_LOG_DEBUG("val: " << *val);
        }

        query.queryValue = value();

        return query;
}

} // namespace NfDesktop
