/**
 * File name: NfBrowserTopBar.cpp
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

#include "NfBrowserTopBar.h"
//#include "NfBrowserBreadcrumbBar.h"
#include "NfBrowserViewModeBar.h"
#include "NfBrowserSortBar.h"
#include "NfBrowserFilterBar.h"
#include "NfUiBrowserState.h"

#include <QHBoxLayout>
#include <QActionGroup>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QToolButton>

namespace NfDesktop {

NfBrowserTopBar::NfBrowserTopBar(NfUi::NfUiBrowserState* state,
                                 QWidget* parent)
        : NfStyledWidget(parent)
        , m_state{state}
        , m_editor{QStringLiteral("rawtherapee")}
{
        setupUi();
}

void NfBrowserTopBar::setupUi()
{
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);

        //        m_breadcrumbBar = new NfBrowserBreadcrumbBar(this);
        m_viewModeBar = new NfBrowserViewModeBar(this);
        m_viewModeBar->setState(m_state);
        m_sortBar = new NfBrowserSortBar(m_state, this);
        m_filterBar = new NfBrowserFilterBar(m_state, this);

        //        layout->addWidget(m_breadcrumbBar,1);
        layout->addWidget(m_viewModeBar, 0, Qt::AlignVCenter);
        layout->addWidget(m_sortBar, 0, Qt::AlignVCenter);
        layout->addWidget(m_filterBar, 0, Qt::AlignVCenter);
        m_editButton = new QToolButton(this);
        m_editButton->setObjectName("editInButton");
        m_editButton->setPopupMode(QToolButton::MenuButtonPopup);

        auto* menu = new QMenu(m_editButton);
        auto* editorGroup = new QActionGroup(menu);
        editorGroup->setExclusive(true);
        const auto addEditor = [this, menu, editorGroup](const QString& name,
                                                        const QString& command) {
                auto* action = menu->addAction(name);
                action->setCheckable(true);
                action->setData(command);
                editorGroup->addAction(action);
                QObject::connect(action, &QAction::triggered, this, [this, action]() {
                        m_editor = action->data().toString();
                        updateEditButton();
                });
                return action;
        };

        addEditor(tr("GIMP"), QStringLiteral("gimp"));
        auto* rawTherapeeAction = addEditor(tr("RawTherapee"),
                                            QStringLiteral("rawtherapee"));
        rawTherapeeAction->setChecked(true);
        m_editButton->setMenu(menu);
        updateEditButton();

        QObject::connect(m_editButton, &QToolButton::clicked,
                         this, &NfBrowserTopBar::openCurrentPhoto);
        QObject::connect(m_state, &NfUi::NfUiBrowserState::currentPhotoChanged,
                         this, &NfBrowserTopBar::updateEditButton);

        layout->addWidget(m_editButton, 0, Qt::AlignVCenter);
}

void NfBrowserTopBar::updateEditButton()
{
        const auto editorName = m_editor == QStringLiteral("gimp")
                ? tr("GIMP")
                : tr("RawTherapee");
        m_editButton->setText(tr("Edit in %1").arg(editorName));
        const auto hasPhoto = !m_state->currentPhoto().path().empty();
        m_editButton->setEnabled(hasPhoto);
        m_editButton->setToolTip(hasPhoto
                                 ? tr("Open the selected photo in %1").arg(editorName)
                                 : tr("Select a photo to edit"));
}

void NfBrowserTopBar::openCurrentPhoto()
{
        const auto& path = m_state->currentPhoto().path();
        if (path.empty())
                return;

        const auto utf8Path = path.u8string();
        const auto photoPath = QString::fromUtf8(
                reinterpret_cast<const char*>(utf8Path.data()),
                static_cast<qsizetype>(utf8Path.size()));
        if (!QProcess::startDetached(m_editor, {photoPath})) {
                const auto editorName = m_editor == QStringLiteral("gimp")
                        ? tr("GIMP")
                        : tr("RawTherapee");
                QMessageBox::warning(this,
                                     tr("Could not open photo"),
                                     tr("Could not launch %1. Make sure it is installed "
                                        "and available in PATH.")
                                             .arg(editorName));
        }
}

} // namespace NfDesktop
