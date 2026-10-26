/**
 * File name: NfSettingsDialog.cpp
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

#include "NfSettingsDialog.h"

#include "core/NfApplicationPaths.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QVBoxLayout>

namespace NfDesktop {

namespace {

QLabel* createDescription(const QString& text, QWidget* parent)
{
        auto* label = new QLabel(text, parent);
        label->setObjectName("SettingsDescription");
        label->setWordWrap(true);
        return label;
}

} // namespace

NfSettingsDialog::NfSettingsDialog(QWidget* parent)
        : QDialog(parent)
        , m_diskCacheSettings{NfCore::NfDiskCacheSettings::load()}
{
        setupUI();
        setupConnections();
        setWindowTitle(tr("Application Preferences"));
        resize(760, 540);
        updateAvailableDiskSpace();
}

void NfSettingsDialog::setupUI()
{
        m_categories = new QListWidget(this);
        m_categories->setObjectName("SettingsCategories");
        m_categories->setFixedWidth(150);
        m_categories->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_categories->setFocusPolicy(Qt::NoFocus);

        m_pages = new QStackedWidget(this);
        m_pages->setObjectName("SettingsPages");

        m_categories->addItem(tr("Performance"));
        m_categories->addItem(tr("File Types"));

        auto* performancePage = new QWidget(this);
        auto* performanceLayout = new QVBoxLayout(performancePage);
        performanceLayout->setContentsMargins(16, 10, 16, 16);
        performanceLayout->setSpacing(12);

        auto* pageTitle = new QLabel(tr("Performance & Resource Limits"), performancePage);
        pageTitle->setObjectName("SettingsPageTitle");
        performanceLayout->addWidget(pageTitle);

        auto* thumbnailGroup = new QGroupBox(tr("Thumbnail Cache Memory Limit"), performancePage);
        auto* thumbnailLayout = new QFormLayout(thumbnailGroup);
        m_thumbnailMemoryLimit = new QSpinBox(thumbnailGroup);
        m_thumbnailMemoryLimit->setRange(64, 65536);
        m_thumbnailMemoryLimit->setValue(2048);
        m_thumbnailMemoryLimit->setSuffix(tr(" MB"));
        m_thumbnailMemoryLimit->setMaximumWidth(180);
        thumbnailLayout->addRow(tr("Size in MB:"), m_thumbnailMemoryLimit);
        thumbnailLayout->addRow(QString{}, createDescription(
                                        tr("Budget for decoded thumbnail images in RAM."),
                                        thumbnailGroup));
        performanceLayout->addWidget(thumbnailGroup);

        auto* previewGroup = new QGroupBox(tr("Preview Cache Memory Limit"), performancePage);
        auto* previewLayout = new QFormLayout(previewGroup);
        m_previewMemoryLimit = new QSpinBox(previewGroup);
        m_previewMemoryLimit->setRange(64, 65536);
        m_previewMemoryLimit->setValue(2048);
        m_previewMemoryLimit->setSuffix(tr(" MB"));
        m_previewMemoryLimit->setMaximumWidth(180);
        previewLayout->addRow(tr("Size in MB:"), m_previewMemoryLimit);
        previewLayout->addRow(QString{}, createDescription(
                                      tr("Kept separate from the thumbnail memory cache."),
                                      previewGroup));
        performanceLayout->addWidget(previewGroup);

        auto* diskGroup = new QGroupBox(tr("Disk Cache"), performancePage);
        auto* diskLayout = new QFormLayout(diskGroup);
        m_diskCacheLimit = new QSpinBox(diskGroup);
        m_diskCacheLimit->setRange(1, 4096);
        m_diskCacheLimit->setValue(static_cast<int>(
                m_diskCacheSettings.maxSizeBytes / (1024ULL * 1024 * 1024)));
        m_diskCacheLimit->setSuffix(tr(" GB"));
        m_diskCacheLimit->setMaximumWidth(180);
        diskLayout->addRow(tr("Disk cache limit:"), m_diskCacheLimit);
        diskLayout->addRow(QString{}, createDescription(
                                   tr("Maximum total size of regenerable cached files."),
                                   diskGroup));

        auto* locationRow = new QWidget(diskGroup);
        auto* locationLayout = new QHBoxLayout(locationRow);
        locationLayout->setContentsMargins(0, 0, 0, 0);
        m_diskCacheLocation = new QLineEdit(locationRow);
        m_diskCacheLocation->setReadOnly(true);
        m_diskCacheLocation->setText(QString::fromStdString(
                m_diskCacheSettings.directory.string()));
        m_browseDiskCache = new QPushButton(tr("Browse…"), locationRow);
        locationLayout->addWidget(m_diskCacheLocation, 1);
        locationLayout->addWidget(m_browseDiskCache);
        diskLayout->addRow(tr("Disk cache location:"), locationRow);

        m_availableDiskSpace = new QLabel(diskGroup);
        m_availableDiskSpace->setObjectName("SettingsDescription");
        diskLayout->addRow(QString{}, m_availableDiskSpace);

        m_minimumFreeDiskSpace = new QSpinBox(diskGroup);
        m_minimumFreeDiskSpace->setRange(1, 4096);
        m_minimumFreeDiskSpace->setValue(static_cast<int>(
                m_diskCacheSettings.minimumFreeSpaceBytes / (1024ULL * 1024 * 1024)));
        m_minimumFreeDiskSpace->setSuffix(tr(" GB"));
        m_minimumFreeDiskSpace->setMaximumWidth(180);
        m_minimumFreeDiskSpace->setToolTip(
                tr("Cache writes stop before the selected cache drive falls below this amount of free space."));
        diskLayout->addRow(tr("Minimum free space on cache drive:"),
                           m_minimumFreeDiskSpace);
        diskLayout->addRow(QString{}, createDescription(tr("A free-space threshold on the selected cache drive; it is not an extra allocation for Neofluxon's cache."),
                                   diskGroup));
        performanceLayout->addWidget(diskGroup);

        performanceLayout->addStretch();

        auto* scrollArea = new QScrollArea(this);
        scrollArea->setObjectName("SettingsScrollArea");
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidget(performancePage);
        m_pages->addWidget(scrollArea);

        auto* fileTypesPage = new QWidget(this);
        auto* fileTypesLayout = new QVBoxLayout(fileTypesPage);
        fileTypesLayout->setContentsMargins(16, 10, 16, 16);
        fileTypesLayout->setSpacing(12);

        auto* formatsTitle = new QLabel(tr("Supported File Types"), fileTypesPage);
        formatsTitle->setObjectName("SettingsPageTitle");
        fileTypesLayout->addWidget(formatsTitle);
        fileTypesLayout->addWidget(createDescription(
                                           tr("Neofluxon currently supports the following image formats."),
                                           fileTypesPage));

        auto* rawGroup = new QGroupBox(tr("RAW"), fileTypesPage);
        auto* rawLayout = new QVBoxLayout(rawGroup);
        for (const auto& format : {".cr3", ".cr2", ".dng", ".raf", ".nef"}) {
                auto* checkBox = new QCheckBox(QString::fromLatin1(format), rawGroup);
                checkBox->setChecked(true);
                rawLayout->addWidget(checkBox);
        }
        fileTypesLayout->addWidget(rawGroup);

        auto* rasterGroup = new QGroupBox(tr("Raster Images"), fileTypesPage);
        auto* rasterLayout = new QVBoxLayout(rasterGroup);
        for (const auto& format : {".jpg", ".jpeg", ".png"}) {
                auto* checkBox = new QCheckBox(QString::fromLatin1(format), rasterGroup);
                checkBox->setChecked(true);
                rasterLayout->addWidget(checkBox);
        }
        fileTypesLayout->addWidget(rasterGroup);
        fileTypesLayout->addStretch();
        m_pages->addWidget(fileTypesPage);
        m_categories->setCurrentRow(0);

        auto* contentLayout = new QHBoxLayout;
        contentLayout->setContentsMargins(0, 0, 0, 0);
        contentLayout->setSpacing(10);
        contentLayout->addWidget(m_categories);
        contentLayout->addWidget(m_pages, 1);

        m_okButton = new QPushButton(tr("OK"), this);
        m_cancelButton = new QPushButton(tr("Cancel"), this);
        auto* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_okButton);
        buttonLayout->addWidget(m_cancelButton);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 12, 12, 12);
        mainLayout->addLayout(contentLayout, 1);
        mainLayout->addLayout(buttonLayout);
}

void NfSettingsDialog::setupConnections()
{
        QObject::connect(m_categories, &QListWidget::currentRowChanged,
                m_pages, &QStackedWidget::setCurrentIndex);

        QObject::connect(m_browseDiskCache, &QPushButton::clicked, this, [this]() {
                const QString directory = QFileDialog::getExistingDirectory(
                        this, tr("Select Disk Cache Folder"),
                        m_diskCacheLocation->text());
                if (!directory.isEmpty()) {
                        m_diskCacheLocation->setText(directory);
                        updateAvailableDiskSpace();
                }
        });

        QObject::connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
        QObject::connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void NfSettingsDialog::accept()
{
        m_diskCacheSettings.directory =
                std::filesystem::path(m_diskCacheLocation->text().toStdString());
        m_diskCacheSettings.maxSizeBytes =
                static_cast<std::uint64_t>(m_diskCacheLimit->value())
                * 1024ULL * 1024 * 1024;
        m_diskCacheSettings.minimumFreeSpaceBytes =
                static_cast<std::uint64_t>(m_minimumFreeDiskSpace->value())
                * 1024ULL * 1024 * 1024;
        std::string error;
        if (!m_diskCacheSettings.save(error)) {
                QMessageBox::warning(this, tr("Disk Cache Settings"), QString::fromStdString(error));
                return;
        }
        QDialog::accept();
}

void NfSettingsDialog::updateAvailableDiskSpace()
{
        const QString path = m_diskCacheLocation->text();
        QStorageInfo storage(path);
        if (!storage.isValid() || !storage.isReady())
                storage = QStorageInfo(QFileInfo(path).absolutePath());

        if (!storage.isValid() || !storage.isReady() || storage.bytesAvailable() < 0) {
                m_availableDiskSpace->setText(tr("Available disk space: unavailable"));
                return;
        }

        constexpr double bytesPerGigabyte = 1024.0 * 1024.0 * 1024.0;
        const double availableGigabytes = storage.bytesAvailable() / bytesPerGigabyte;
        m_availableDiskSpace->setText(
                tr("Available disk space: %1 GB")
                        .arg(QLocale().toString(availableGigabytes, 'f', 1)));
}

} // namespace NfDesktop
