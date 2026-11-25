/**
 * File name: NfNewProjectDialog.h
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

#ifndef NF_NEW_PROJECT_DIALOG_H
#define NF_NEW_PROJECT_DIALOG_H

#include <QDialog>
#include <QString>

#include <filesystem>

class QLineEdit;
class QTextEdit;
class QDialogButtonBox;
class QPushButton;

namespace NfUi {
class NfContext;
}

namespace NfDesktop {

class NfNewProjectDialog: public QDialog
{
        Q_OBJECT

 public:
        explicit NfNewProjectDialog(const NfUi::NfContext& ctx,
                                    QWidget* parent = nullptr);

        QString name() const;
        QString description() const;
        std::filesystem::path path() const;

 private:
        void browse();
        void updatePath();
        void accept() override;
        bool isValidProjectName(const QString& name) const;

        QLineEdit* m_nameEdit{nullptr};
        QTextEdit* m_descriptionEdit{nullptr};
        QLineEdit* m_pathEdit{nullptr};
        QPushButton* m_browseButton{nullptr};
        QDialogButtonBox* m_buttonBox{nullptr};
        std::filesystem::path m_parentPath;
};

} // namespace NfDesktop

#endif // NF_NEW_PROJECT_DIALOG_H
