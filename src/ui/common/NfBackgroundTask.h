/**
 * File name: NfBackgroundTask.h
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

#ifndef NF_BACKGROUND_TASK_H
#define NF_BACKGROUND_TASK_H

#include <QString>

namespace NfUi {

struct NfBackgroundTask
{
        enum class State {
                Running,
                Paused,
                Stopped
        };

        QString id;
        QString operation;
        QString sourcePath;
        State state;
        int progress;
        QString progressText;
        QString actionText;
};

} // namespace NfUi

#endif // NF_BACKGROUND_TASK_H
