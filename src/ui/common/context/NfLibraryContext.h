/**
 * File name: NfLibraryContext.h
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

#ifndef NF_LIBRARY_CONTEXT_H
#define NF_LIBRARY_CONTEXT_H

#include "NfContext.h"

namespace NfUi {

struct NfContext;
class NfUiState;
class NfLibraryAdapter;

class NfLibraryContext {
 public:
        explicit NfLibraryContext(NfContext* ctx)
                : m_context{ctx} {}
        NfUiState* uiState() const { return m_context->uiState; }
        NfContext* prent() const { return m_context; }
        NfLibraryAdapter* library() const { return m_context->library; }

 private:
        NfContext* m_context;
};

} // namespace NfUi

#endif // NF_LIBRARY_CONTEXT_H
