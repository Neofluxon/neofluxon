/**
 * File name: NfUserTaskData.h
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
 */

#ifndef NF_USER_TASK_DATA_H
#define NF_USER_TASK_DATA_H

#include <any>

namespace NfCore {

enum class NfUserTaskType {
        ImportFolder
};

struct NfUserTaskData {
        NfUserTaskType type;
        std::any taskData;
};

} // namespace NfCore

#endif // NF_USER_TASK_DATA_H
