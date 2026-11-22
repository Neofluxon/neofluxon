/**
 * File name: NfSearchCriteriaWidget.h
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

#ifndef NF_SEARCH_CRITERIA_WIDGET_H
#define NF_SEARCH_CRITERIA_WIDGET_H

#include "NfStyledWidget.h"
#include "core/library/NfLibraryQuery.h"

#include <QString>
#include <QStringList>

#include <vector>

class QVBoxLayout;

namespace NfDesktop {

class NfDesktopContext;

class NfSearchCriteriaWidget : public NfStyledWidget
{
        Q_OBJECT

public:
        NfSearchCriteriaWidget(const NfDesktopContext &ctx,
                               QWidget* parent = nullptr);
        ~NfSearchCriteriaWidget() override = default;
        std::vector<NfCore::NfLibrarySearchCriterion> criteria() const;

signals:
        void searchRequested();

protected slots:
        void clearCriteria();

private:
        struct CriterionRow
        {
                NfCore::NfLibrarySearchField field;
                QWidget* widget{nullptr};
        };

        void addCriterion(const QString& name);

        const NfDesktopContext& m_context;
        QWidget* m_criteriaListWidget{nullptr};
        QVBoxLayout* m_criteriaLayout{nullptr};
        QStringList m_cameraValues;
        QStringList m_lensValues;
        std::vector<CriterionRow> m_criteria;
};

} // namespace NfDesktop

#endif // NF_SEARCH_CRITERIA_WIDGET_H
