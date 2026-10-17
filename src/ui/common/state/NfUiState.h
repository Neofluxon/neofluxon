/**
 * File name: NfUiState.h
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

#ifndef NF_UI_STATE_H
#define NF_UI_STATE_H

#include <QObject>
#include <QHash>
#include <QString>

namespace NfUi {

enum class NfUiMode
{
        Shoots,
        Folders,
        Library
};

class NfUiShootsModeState;
class NfUiFolderModeState;
class NfUiLibraryModeState;

class NfUiState : public QObject
{
        Q_OBJECT

public:
        explicit NfUiState(QObject* parent = nullptr);
        NfUiShootsModeState* shootsModeState() const;
        NfUiFolderModeState* folderModeState() const;
        NfUiLibraryModeState* libraryModeState() const;

        NfUiMode mode() const;
        bool metadataSectionExpanded(const QString& section) const;
        void setMetadataSectionExpanded(const QString& section, bool expanded);

public slots:
        void setMode(NfUiMode mode);

signals:
        void modeChanged(NfUiMode mode);

private:
        void saveState();

        NfUiMode m_uiMode;
        NfUiShootsModeState *m_shootsModeState;
        NfUiFolderModeState *m_folderModeState;
        NfUiLibraryModeState *m_libraryModeState;
        QHash<QString, bool> m_metadataSectionStates;
};

} // namespace NfUi

#endif // NF_UI_STATE_H
