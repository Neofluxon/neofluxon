/**
 * File name: NfLibraryTreeTask.cpp
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
 * Neofluxon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "NfLibraryTreeTask.h"
#include "NfLibraryManager.h"
#include "NfLibrary.h"
#include "NfLogger.h"

#include <exception>

namespace NfCore {

NfLibraryTreeTask::NfLibraryTreeTask(NfLibraryManager* libraryManager)
        : m_libraryManager{libraryManager}
{
}

NfLibraryTreeTask::NfLibraryTreeTask(NfLibraryManager* libraryManager, int64_t libraryId)
        : m_libraryManager{libraryManager}
        , m_libraryId{libraryId}
{
}

NfTask::TaskStatus NfLibraryTreeTask::execute()
{
        try {
                if (m_libraryId) {
                        if (isCancelled())
                                return TaskStatus::Cancelled;

                        auto library = m_libraryManager->getLibrary(*m_libraryId);
                        if (!library)
                                return TaskStatus::Failed;

                        auto snapshot = snapshotLibrary(*library);
                        if (isCancelled())
                                return TaskStatus::Cancelled;
                        m_snapshot.push_back(std::move(snapshot));
                        return TaskStatus::Success;
                }

                const auto libraries = m_libraryManager->libraries();
                for (const auto& library : libraries) {
                        if (isCancelled())
                                return TaskStatus::Cancelled;

                        auto snapshot = snapshotLibrary(*library);
                        if (isCancelled())
                                return TaskStatus::Cancelled;
                        m_snapshot.push_back(std::move(snapshot));
                }
        } catch (const std::exception& exception) {
                NF_LOG_ERROR("can't build library tree: " << exception.what());
                return TaskStatus::Failed;
        }

        return TaskStatus::Success;
}

NfLibraryTreeSnapshot NfLibraryTreeTask::takeSnapshot()
{
        return std::move(m_snapshot);
}

NfLibrarySnapshot NfLibraryTreeTask::snapshotLibrary(const NfLibrary& library) const
{
        NfLibrarySnapshot snapshot{
                .id = library.id(),
                .name = library.name()
        };
        for (const auto& representation : library.representations()) {
                if (isCancelled())
                        break;

                NfLibraryRepresentationSnapshot representationSnapshot{
                        .name = representation->name(),
                        .type = representation->type()
                };
                if (const auto* tree = representation->getTree()) {
                        for (const auto& child : tree->children()) {
                                if (isCancelled())
                                        break;
                                representationSnapshot.children.push_back(snapshotNode(*child));
                        }
                }
                snapshot.representations.push_back(std::move(representationSnapshot));
        }
        return snapshot;
}

NfLibraryTreeNodeSnapshot
NfLibraryTreeTask::snapshotNode(const NfLibraryTreeNode& node)
{
        NfLibraryTreeNodeSnapshot snapshot{
                .name = node.name(),
                .type = node.type(),
                .value = node.getValue()
        };
        for (const auto& child : node.children())
                snapshot.children.push_back(snapshotNode(*child));
        return snapshot;
}

} // namespace NfCore
