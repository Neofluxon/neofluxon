/**
 * File name: NfLibraryAdapter.h
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

#ifndef NF_LIBRARY_ADAPTER_H
#define NF_LIBRARY_ADAPTER_H

#include <QObject>

#include <filesystem>
#include <string_view>
#include <vector>
#include <memory>

namespace NfCore {
class NfLibrary;
class NfLibraryManager;
}

namespace NfUi {

class NfLibraryAdapter : public QObject
{
        Q_OBJECT

public:
        explicit NfLibraryAdapter(NfCore::NfLibraryManager *library,
                                  QObject* parent = nullptr);
        ~NfLibraryAdapter();
        void addLibrary(std::string_view name);
        void importPath(const std::filesystem::path &path, uint64_t libraryId = 0);
        std::vector<std::unique_ptr<NfCore::NfLibrary>> libraries() const;

signals:
        void folderImported();

private:
        NfCore::NfLibraryManager *m_library;
};

} // namespace NfUi

#endif // NF_LIBRARY_ADAPTER_H

