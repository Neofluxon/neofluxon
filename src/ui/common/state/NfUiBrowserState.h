/**
 * File name: NfBrowserState.h
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

#ifndef NF_UI_BROWSER_STATE_H
#define NF_UI_BROWSER_STATE_H

#include <QObject>

namespace NfUi {

class NfUiBrowserState : public QObject
{
        Q_OBJECT

public:
        enum class ViewMode {
                Grid,
                Preview
        };

        explicit NfUiBrowserState(QObject* parent = nullptr);
        ViewMode viewMode() const;

signals:
        void viewModeChanged(ViewMode mode);

public slots:
        void setViewMode(ViewMode mode);

private:
        ViewMode m_viewMode;
};
} // namespace NfUi

#endif // NF_UI_BROWSER_STATE_H
