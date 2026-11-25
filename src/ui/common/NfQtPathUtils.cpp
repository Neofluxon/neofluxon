/**
 * File name: NfQtPathUtils.cpp
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

#include "NfQtPathUtils.h"

namespace NfUi {

#ifdef Q_OS_WIN
std::filesystem::path NfQtPathUtils::toPath(const QString& path)
{
        return std::filesystem::path(path.toStdWString());
}

QString NfQtPathUtils::fromPath(const std::filesystem::path& path)
{
        return QString::fromStdWString(path.wstring());
}
#else
std::filesystem::path NfQtPathUtils::toPath(const QString& path)
{
        const QByteArray utf8 = path.toUtf8();
        return std::filesystem::path(utf8.constData());
}

QString NfQtPathUtils::fromPath(const std::filesystem::path& path)
{
    const auto utf8 = path.u8string();
    return QString::fromUtf8(reinterpret_cast<const char*>(utf8.data()),
                             static_cast<qsizetype>(utf8.size()));
}

} // namespace NfUi

#endif
