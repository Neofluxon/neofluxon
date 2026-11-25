/**
 * File name: NfNewProjectDialog.cpp
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

#include "NfNewProjectDialog.h"
#include "NfContext.h"
#include "NfQtPathUtils.h"
#include "core/NfSettings.h"
#include "core/NeofluxonCore.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QRegularExpression>

using namespace NfUi;
using namespace NfCore;

namespace NfDesktop {

NfNewProjectDialog::NfNewProjectDialog(const NfContext& ctx, QWidget* parent)
        : QDialog(parent)
{
        setWindowTitle(tr("New Project"));
        setModal(true);
        setMinimumWidth(440);

        auto* mainLayout = new QVBoxLayout(this);
        auto* formLayout = new QFormLayout;

        m_nameEdit = new QLineEdit(this);
        m_nameEdit->setPlaceholderText(tr("Project name"));
        m_nameEdit->setText(tr("New Project"));

        m_descriptionEdit = new QTextEdit(this);
        m_descriptionEdit->setPlaceholderText(tr("Enter a project description..."));
        m_descriptionEdit->setFixedHeight(75);

        formLayout->addRow(tr("Project name:"), m_nameEdit);
        formLayout->addRow(tr("Description:"), m_descriptionEdit);

        mainLayout->addLayout(formLayout);

        auto* locationLayout = new QVBoxLayout;
        auto* locationLabel = new QLabel(tr("Project location:"), this);
        auto* pathLayout = new QHBoxLayout;

        m_pathEdit = new QLineEdit(this);
        m_pathEdit->setReadOnly(true);

        m_browseButton = new QPushButton(tr("Browse..."), this);

        pathLayout->addWidget(m_pathEdit);
        pathLayout->addWidget(m_browseButton);

        locationLayout->addWidget(locationLabel);
        locationLayout->addLayout(pathLayout);

        mainLayout->addLayout(locationLayout);

        m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel |
                                           QDialogButtonBox::Ok,
                                           this);
        auto* dialogButton = m_buttonBox->button(QDialogButtonBox::Ok);
        dialogButton->setText(tr("Create project"));
        dialogButton->setIcon(QIcon());
        mainLayout->addWidget(m_buttonBox);

        dialogButton = m_buttonBox->button(QDialogButtonBox::Cancel);
        dialogButton->setText(tr("Cancel"));
        dialogButton->setIcon(QIcon());
        mainLayout->addWidget(m_buttonBox);

        m_parentPath = ctx.core()->settings().projectsPath;

        QObject::connect(m_nameEdit,
                         &QLineEdit::textChanged,
                         this,
                         &NfNewProjectDialog::updatePath);
        QObject::connect(m_browseButton,
                         &QPushButton::clicked,
                         this,
                         &NfNewProjectDialog::browse);
        QObject::connect(m_buttonBox,
                         &QDialogButtonBox::accepted,
                         this,
                         &NfNewProjectDialog::accept);
        QObject::connect(m_buttonBox,
                         &QDialogButtonBox::rejected,
                         this,
                         &NfNewProjectDialog::reject);

        updatePath();
}

QString NfNewProjectDialog::name() const
{
        return m_nameEdit->text().trimmed();
}

QString NfNewProjectDialog::description() const
{
        return m_descriptionEdit->toPlainText().trimmed();
}

std::filesystem::path NfNewProjectDialog::path() const
{
        return m_parentPath;
}

void NfNewProjectDialog::updatePath()
{
        m_pathEdit->setText(NfQtPathUtils::fromPath(path()));
}

void NfNewProjectDialog::browse()
{
        const auto directory = QFileDialog::getExistingDirectory(this,
                                                                 tr("Select Project Location"),
                                                                 NfQtPathUtils::fromPath(m_parentPath));

        if (directory.isEmpty())
                return;

        m_parentPath = NfQtPathUtils::toPath(directory);

        updatePath();
}

void NfNewProjectDialog::accept()
{
        const auto projectName = name();
        if (!isValidProjectName(projectName)) {
                QMessageBox::warning(
                                     this,
                                     tr("Invalid Project Name"),
                                     tr("Please enter a valid project name."));
                m_nameEdit->setFocus();
                return;
        }

        std::error_code ec;
        if (std::filesystem::exists(path(), ec)) {
                QMessageBox::warning(
                                     this,
                                     tr("Project Already Exists"),
                                     tr("A directory with this project name "
                                        "already exists at the selected location."));
                return;
        }

        QDialog::accept();
}

bool NfNewProjectDialog::isValidProjectName(const QString& name) const
{
        const QString value = name.trimmed();
        if (value.isEmpty())
                return false;

        if (value.size() > 100)
                return false;

        const QRegularExpression invalidChars(R"([<>:"/\\|?*\x00-\x1F])");
        if (value.contains(invalidChars))
                return false;

        if (value.endsWith('.'))
                return false;

        const QRegularExpression reservedNames(
                   R"(^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\..*)?$)",
                   QRegularExpression::CaseInsensitiveOption);
        if (value.contains(reservedNames))
                return false;

        return true;
}

} // namespace NfDesktop

