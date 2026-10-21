/**
 * File name: NfUiState.cpp
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

#include "NfUiState.h"
#include "NfUiShootsModeState.h"
#include "NfUiFolderModeState.h"
#include "NfUiLibraryModeState.h"
#include "NfUiBrowserState.h"
#include "core/NfApplicationPaths.h"
#include "core/library/NfLibraryRepresentation.h"
#include "core/utils/NfFileUtils.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include <variant>

using namespace NfCore;

namespace NfUi {

namespace {

QString pathToString(const std::filesystem::path& path)
{
        const auto utf8Path = path.u8string();
        return QString::fromUtf8(reinterpret_cast<const char*>(utf8Path.data()),
                                 static_cast<qsizetype>(utf8Path.size()));
}

QString settingsFilePath()
{
        const auto configPath = NfApplicationPaths().configDirectory() /
                                "ui-state.ini";
        return pathToString(configPath);
}

std::filesystem::path stringToPath(const QString& path)
{
        const auto utf8Path = path.toUtf8();
        return NfFileUtils::pathFromUtf8(
                std::string_view{utf8Path.constData(),
                                 static_cast<std::size_t>(utf8Path.size())});
}

void saveBrowserState(QSettings& settings,
                      const QString& key,
                      const NfUiBrowserState* state)
{
        settings.setValue(key + QStringLiteral("/viewMode"),
                          static_cast<int>(state->viewMode()));
        settings.setValue(key + QStringLiteral("/currentPhoto"),
                          pathToString(state->currentPhoto().path()));
}

void restoreBrowserState(QSettings& settings,
                         const QString& key,
                         NfUiBrowserState* state)
{
        const auto viewMode = settings.value(key + QStringLiteral("/viewMode"),
                                             static_cast<int>(NfUiBrowserState::ViewMode::Grid)).toInt();
        if (viewMode == static_cast<int>(NfUiBrowserState::ViewMode::Preview))
                state->setViewMode(NfUiBrowserState::ViewMode::Preview);
        else
                state->setViewMode(NfUiBrowserState::ViewMode::Grid);

        const auto photoPath = settings.value(key + QStringLiteral("/currentPhoto")).toString();
        if (!photoPath.isEmpty())
                state->setCurrentPhoto(NfPhoto{stringToPath(photoPath)});
}

void saveLibraryQuery(QSettings& settings, const NfLibraryQuery& query)
{
        const auto key = QStringLiteral("ui/library/query");
        settings.remove(key);
        settings.setValue(key + QStringLiteral("/libraryId"),
                          QString::number(static_cast<qlonglong>(query.libraryId)));
        settings.setValue(key + QStringLiteral("/representationType"),
                          static_cast<int>(query.representationType));

        const auto& value = query.queryValue;
        if (std::holds_alternative<std::monostate>(value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("none"));
        } else if (const auto* intValue = std::get_if<int>(&value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("int"));
                settings.setValue(key + QStringLiteral("/value"), *intValue);
        } else if (const auto* int64Value = std::get_if<int64_t>(&value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("int64"));
                settings.setValue(key + QStringLiteral("/value"),
                                  QString::number(static_cast<qlonglong>(*int64Value)));
        } else if (const auto* stringValue = std::get_if<std::string>(&value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("string"));
                settings.setValue(key + QStringLiteral("/value"),
                                  QString::fromStdString(*stringValue));
        } else if (const auto* dateRange = std::get_if<NfDateRange>(&value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("dateRange"));
                settings.setValue(key + QStringLiteral("/startTicks"),
                                  QString::number(static_cast<qlonglong>(dateRange->start_ticks)));
                settings.setValue(key + QStringLiteral("/endTicks"),
                                  QString::number(static_cast<qlonglong>(dateRange->end_ticks)));
        } else if (const auto* equipment = std::get_if<NfEquipmentQuery>(&value)) {
                settings.setValue(key + QStringLiteral("/valueType"), QStringLiteral("equipment"));
                settings.setValue(key + QStringLiteral("/equipmentKind"),
                                  static_cast<int>(equipment->kind));
                settings.setValue(key + QStringLiteral("/value"),
                                  QString::number(static_cast<qlonglong>(equipment->id)));
        }
}

void restoreLibraryQuery(QSettings& settings, NfUiLibraryModeState* state)
{
        using RepresentationType = NfLibraryRepresentation::RepresentationType;
        const auto key = QStringLiteral("ui/library/query");
        const auto representationKey = key + QStringLiteral("/representationType");
        if (!settings.contains(representationKey))
                return;

        bool ok = false;
        const auto libraryId = settings.value(key + QStringLiteral("/libraryId"))
                                              .toString().toLongLong(&ok);
        if (!ok)
                return;

        const auto rawRepresentation = settings.value(representationKey).toInt(&ok);
        if (!ok || rawRepresentation < static_cast<int>(RepresentationType::None)
            || rawRepresentation > static_cast<int>(RepresentationType::Collections))
                return;

        NfLibraryQuery query;
        query.libraryId = libraryId;
        query.representationType = static_cast<RepresentationType>(rawRepresentation);

        const auto valueType = settings.value(key + QStringLiteral("/valueType")).toString();
        if (valueType == QStringLiteral("int")) {
                const auto value = settings.value(key + QStringLiteral("/value")).toInt(&ok);
                if (!ok)
                        return;
                query.queryValue = value;
        } else if (valueType == QStringLiteral("int64")) {
                const auto value = settings.value(key + QStringLiteral("/value"))
                                           .toString().toLongLong(&ok);
                if (!ok)
                        return;
                query.queryValue = static_cast<int64_t>(value);
        } else if (valueType == QStringLiteral("string")) {
                query.queryValue = settings.value(key + QStringLiteral("/value"))
                                           .toString().toStdString();
        } else if (valueType == QStringLiteral("dateRange")) {
                const auto start = settings.value(key + QStringLiteral("/startTicks"))
                                           .toString().toLongLong(&ok);
                if (!ok)
                        return;
                const auto end = settings.value(key + QStringLiteral("/endTicks"))
                                         .toString().toLongLong(&ok);
                if (!ok)
                        return;
                query.queryValue = NfDateRange{start, end};
        } else if (valueType == QStringLiteral("equipment")) {
                const auto rawKind = settings.value(key + QStringLiteral("/equipmentKind")).toInt(&ok);
                if (!ok || (rawKind != static_cast<int>(NfEquipmentQuery::Kind::Camera)
                            && rawKind != static_cast<int>(NfEquipmentQuery::Kind::Lens)))
                        return;
                const auto id = settings.value(key + QStringLiteral("/value"))
                                        .toString().toLongLong(&ok);
                if (!ok)
                        return;
                query.queryValue = NfEquipmentQuery{
                        static_cast<NfEquipmentQuery::Kind>(rawKind), id};
        } else if (valueType != QStringLiteral("none")) {
                return;
        }

        state->setQuery(query);
}

} // namespace

NfUiState::NfUiState(QObject* parent)
        : QObject(parent)
        , m_uiMode{NfUiMode::Shoots}
        , m_shootsModeState{new NfUiShootsModeState(this)}
        , m_folderModeState{new NfUiFolderModeState(this)}
        , m_libraryModeState{new NfUiLibraryModeState(this)}
{
        QSettings settings(settingsFilePath(), QSettings::IniFormat);

        const auto savedMode = settings.value(QStringLiteral("ui/mode"),
                                              static_cast<int>(NfUiMode::Shoots)).toInt();
        if (savedMode >= static_cast<int>(NfUiMode::Shoots) &&
            savedMode <= static_cast<int>(NfUiMode::Library))
                m_uiMode = static_cast<NfUiMode>(savedMode);

        const auto folderPath = settings.value(QStringLiteral("ui/folders/path")).toString();
        if (!folderPath.isEmpty())
                m_folderModeState->setPath(stringToPath(folderPath));

        bool ok = false;
        const auto currentLibrary = settings.value(QStringLiteral("ui/library/currentId"))
                                            .toString().toULongLong(&ok);
        if (ok)
                m_libraryModeState->setCurrentLibrary(currentLibrary);

        restoreLibraryQuery(settings, m_libraryModeState);
        restoreBrowserState(settings, QStringLiteral("ui/folders/browser"),
                            m_folderModeState->browser());
        restoreBrowserState(settings, QStringLiteral("ui/library/browser"),
                            m_libraryModeState->browser());

        settings.beginGroup(QStringLiteral("ui/metadata/sections"));
        for (const auto& section : settings.allKeys())
                m_metadataSectionStates.insert(section, settings.value(section).toBool());
        settings.endGroup();

        QObject::connect(this, &NfUiState::modeChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_folderModeState, &NfUiFolderModeState::pathChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_libraryModeState, &NfUiLibraryModeState::currentLibraryChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_libraryModeState, &NfUiLibraryModeState::queryChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_folderModeState->browser(), &NfUiBrowserState::viewModeChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_libraryModeState->browser(), &NfUiBrowserState::viewModeChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_folderModeState->browser(), &NfUiBrowserState::currentPhotoChanged,
                         this, &NfUiState::saveState);
        QObject::connect(m_libraryModeState->browser(), &NfUiBrowserState::currentPhotoChanged,
                         this, &NfUiState::saveState);
}

NfUiShootsModeState* NfUiState::shootsModeState() const
{
        return m_shootsModeState;
}

NfUiFolderModeState* NfUiState::folderModeState() const
{
        return m_folderModeState;
}

NfUiLibraryModeState* NfUiState::libraryModeState() const
{
        return m_libraryModeState;
}

NfUiMode NfUiState::mode() const
{
        return m_uiMode;
}

bool NfUiState::metadataSectionExpanded(const QString& section) const
{
        return m_metadataSectionStates.value(section, false);
}

void NfUiState::setMetadataSectionExpanded(const QString& section, bool expanded)
{
        if (m_metadataSectionStates.value(section, false) == expanded &&
            m_metadataSectionStates.contains(section))
                return;

        m_metadataSectionStates.insert(section, expanded);
        saveState();
}

void NfUiState::saveState()
{
        const auto path = settingsFilePath();
        const auto directory = QFileInfo(path).absolutePath();
        if (!QDir().mkpath(directory)) {
                qWarning() << "Could not create UI settings directory:" << directory;
                return;
        }

        QSettings settings(path, QSettings::IniFormat);
        settings.setValue(QStringLiteral("ui/mode"), static_cast<int>(m_uiMode));
        settings.setValue(QStringLiteral("ui/folders/path"),
                          pathToString(m_folderModeState->path()));
        settings.setValue(QStringLiteral("ui/library/currentId"),
                          QString::number(static_cast<qulonglong>(
                                  m_libraryModeState->currentLibrary())));
        saveLibraryQuery(settings, m_libraryModeState->query());
        saveBrowserState(settings, QStringLiteral("ui/folders/browser"),
                         m_folderModeState->browser());
        saveBrowserState(settings, QStringLiteral("ui/library/browser"),
                         m_libraryModeState->browser());
        settings.remove(QStringLiteral("ui/metadata/sections"));
        for (auto it = m_metadataSectionStates.cbegin();
             it != m_metadataSectionStates.cend(); ++it) {
                settings.setValue(QStringLiteral("ui/metadata/sections/") + it.key(),
                                  it.value());
        }

        settings.sync();

        if (settings.status() != QSettings::NoError)
                qWarning() << "Could not save UI state to" << path;
}

void NfUiState::setMode(NfUiMode mode)
{
        if (m_uiMode == mode)
                return;

        m_uiMode = mode;
        emit modeChanged(m_uiMode);
}

} // namespace NfUi
