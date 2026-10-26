/**
 * File name: NfSettingsDialog.h
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

#ifndef NF_SETTINGS_DIALOG_H
#define NF_SETTINGS_DIALOG_H

#include <QDialog>

#include "core/NfDiskCacheSettings.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QStackedWidget;

namespace NfDesktop {

class NfSettingsDialog : public QDialog
{
        Q_OBJECT

public:
        explicit NfSettingsDialog(QWidget* parent = nullptr);

private:
        void setupUI();
        void setupConnections();
        void updateAvailableDiskSpace();
        void accept() override;

        NfCore::NfDiskCacheSettings m_diskCacheSettings;
        QListWidget* m_categories{nullptr};
        QStackedWidget* m_pages{nullptr};
        QSpinBox* m_thumbnailMemoryLimit{nullptr};
        QSpinBox* m_previewMemoryLimit{nullptr};
        QSpinBox* m_diskCacheLimit{nullptr};
        QLineEdit* m_diskCacheLocation{nullptr};
        QLabel* m_availableDiskSpace{nullptr};
        QSpinBox* m_minimumFreeDiskSpace{nullptr};
        QPushButton* m_browseDiskCache{nullptr};
        QPushButton* m_okButton{nullptr};
        QPushButton* m_cancelButton{nullptr};
};

} // namespace NfDesktop

#endif // NF_SETTINGS_DIALOG_H
