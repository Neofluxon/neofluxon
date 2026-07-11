/**
 * File name: NfLibraryModel.h
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

#ifndef NF_LIBRARY_MODEL_H
#define NF_LIBRARY_MODEL_H

#include "NfLibraryContext.h"

#include <QObject>

namespace NfCore {
class NfLibraryQuery;
}

namespace NfUi {
class NfBrowserModel;
}

namespace NfDesktop {

class NfLibraryModel : public QObject
{
        Q_OBJECT

public:
        explicit NfLibraryModel(NfUi::NfLibraryContext ctx, QObject *parent = nullptr);
        ~NfFolderModel() = default;
        void setQuery(const NfCore::NfLibraryQuery &query);
        NfBrowserModel* browser() const;

 private:
        NfUi::NfLibraryContext m_context;
        NfBrowserModel *m_browserModel;
};

} // namespace NfDesktop

#endif // NF_LIBRARY_MODEL_H
