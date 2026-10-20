/**
 * File name: NfDesktopFileManagerUtils.cpp
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

#include "NfDesktopFileManagerUtils.h"

#include <QDir>
#include <QStringList>
#include <QUrl>

#if defined(Q_OS_WIN)
#include <QProcess>
#elif defined(Q_OS_LINUX)
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#endif

namespace NfDesktop {

bool NfDesktopFileManagerUtils::revealFile(const QString& path)
{
#if defined(Q_OS_WIN)
        return QProcess::startDetached(
                QStringLiteral("explorer.exe"),
                {QStringLiteral("/select,") + QDir::toNativeSeparators(path)});
#elif defined(Q_OS_LINUX)
        QDBusInterface fileManager(QStringLiteral("org.freedesktop.FileManager1"),
                                   QStringLiteral("/org/freedesktop/FileManager1"),
                                   QStringLiteral("org.freedesktop.FileManager1"),
                                   QDBusConnection::sessionBus());
        if (!fileManager.isValid())
                return false;

        const auto reply = fileManager.call(
                QStringLiteral("ShowItems"),
                QStringList{QUrl::fromLocalFile(path).toString()},
                QString{});
        return reply.type() != QDBusMessage::ErrorMessage;
#else
        Q_UNUSED(path);
        return false;
#endif
}

} // namespace NfDesktop
