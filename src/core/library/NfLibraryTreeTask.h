/**
 * File name: NfLibraryTreeTask.h
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

#ifndef NF_LIBRARY_TREE_TASK_H
#define NF_LIBRARY_TREE_TASK_H

#include "NfTask.h"
#include "NfLibraryRepresentation.h"
#include "NfLibraryTreeSnapshot.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace NfCore {

class NfLibraryManager;
class NfLibrary;

using NfLibraryTreeSnapshot = std::vector<NfLibrarySnapshot>;
using NfLibraryTreeResultHandler =
        std::function<void(NfTask::TaskStatus, NfLibraryTreeSnapshot)>;

class NfLibraryTreeTask : public NfTask {
public:
        explicit NfLibraryTreeTask(NfLibraryManager* libraryManager);
        NfLibraryTreeTask(NfLibraryManager* libraryManager, int64_t libraryId);
        TaskStatus execute() override;
        NfLibraryTreeSnapshot takeSnapshot();

private:
        NfLibrarySnapshot snapshotLibrary(const NfLibrary& library) const;
        static NfLibraryTreeNodeSnapshot snapshotNode(const NfLibraryTreeNode& node);

        NfLibraryManager* m_libraryManager;
        std::optional<int64_t> m_libraryId;
        NfLibraryTreeSnapshot m_snapshot;
};

} // namespace NfCore

#endif // NF_LIBRARY_TREE_TASK_H
