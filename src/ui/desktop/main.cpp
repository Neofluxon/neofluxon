/**
 * File name: main.cpp
 * Project: Neofluxon (A photography workflow software)
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

#include "NfMainWindow.h"
#include "core/NeofluxonCore.h"
#include "NfApplication.h"
#include "NfUiState.h"
#include "NfContext.h"
#include "core/NfLogger.h"
#include "core/NfPhotoId.h"
#include "core/NfPhoto.h"

#include <QMessageBox>

#include <cstdlib>
#include <string>
#include <vector>

using namespace NfCore;
using namespace NfUi;
using namespace NfDesktop;

int main(int argc, char **argv)
{
        auto coreApp = std::make_unique<NeofluxonCore>();

        NfApplication app(coreApp.get(), argc, argv);

        std::string initializationError;
        if (!coreApp->initialize(initializationError)) {
                QMessageBox::critical(nullptr,
                                      "Neofluxon startup error",
                                      QString::fromStdString(initializationError));
                return EXIT_FAILURE;
        }

        qRegisterMetaType<NfPhoto>("NfPhoto");
        qRegisterMetaType<NfPhotoId>("NfPhotoId");
        qRegisterMetaType<std::vector<NfPhoto>>("std::vector<NfPhoto>");
        qRegisterMetaType<std::vector<NfPhotoId>>("std::vector<NfPhotoId>");

        NfUiState uiState;
        NfContext context(coreApp.get(), &uiState);

        NfMainWindow mainWin(&context);
        mainWin.show();

        return app.exec();
}
