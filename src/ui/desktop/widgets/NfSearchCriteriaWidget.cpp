/**
 * File name: NfSearchCriteriaWidget.cpp
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

#include "NfSearchCriteriaWidget.h"

#include <algorithm>

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

namespace NfDesktop {

namespace {

struct CriterionDefinition
{
        QString name;
        NfCore::NfLibrarySearchField field;
        QStringList operators;
        QStringList values;
        bool editable{false};
        bool date{false};
};

const QVector<CriterionDefinition>& criterionDefinitions()
{
        static const QVector<CriterionDefinition> definitions{
                {QObject::tr("Camera"), NfCore::NfLibrarySearchField::Camera,
                 {QObject::tr("Is"), QObject::tr("Contains")},
                 {},
                 true,
                 false},
                {QObject::tr("Lens"), NfCore::NfLibrarySearchField::Lens,
                 {QObject::tr("Is"), QObject::tr("Contains")},
                 {},
                 true,
                 false},
                {QObject::tr("Date Taken"), NfCore::NfLibrarySearchField::DateTaken,
                 {QObject::tr("Is"), QObject::tr("Before"),
                  QObject::tr("After"), QObject::tr("Between")},
                 {},
                 false,
                 true},
                {QObject::tr("File Format"), NfCore::NfLibrarySearchField::FileFormat,
                 {QObject::tr("Is")},
                 {QObject::tr("RAW"), QObject::tr("JPEG"), QObject::tr("PNG")},
                 false,
                 false},
                {QObject::tr("Folder"), NfCore::NfLibrarySearchField::Folder,
                 {QObject::tr("Is"), QObject::tr("Contains")},
                 {},
                 true,
                 false},
        };
        return definitions;
}

const CriterionDefinition* definitionFor(const QString& name)
{
        const auto& definitions = criterionDefinitions();
        const auto it = std::find_if(definitions.cbegin(),
                                     definitions.cend(),
                                     [&name](const auto& definition) {
                                             return definition.name == name;
                                     });
        return it == definitions.cend() ? nullptr : &*it;
}

QWidget* createValueControl(const CriterionDefinition& definition,
                            const QString& op,
                            const QStringList& cameraValues,
                            const QStringList& lensValues,
                            QWidget* parent)
{
        auto* container = new QWidget(parent);
        auto* layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(8);

        const auto addDateEdit = [layout, container]() {
                auto* edit = new QDateEdit(container);
                edit->setCalendarPopup(true);
                edit->setDisplayFormat("yyyy-MM-dd");
                edit->setSpecialValueText(QObject::tr("Select date"));
                edit->setDate(edit->minimumDate());
                edit->setMinimumWidth(128);
                layout->addWidget(edit);
        };

        if (definition.date) {
                addDateEdit();
                if (op == QObject::tr("Between"))
                        addDateEdit();
                return container;
        }

        auto* value = new QComboBox(container);
        value->setObjectName("SearchCriterionInput");
        value->setMinimumWidth(220);
        value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        if (definition.editable)
                value->setEditable(true);
        auto values = definition.values;
        if (definition.field == NfCore::NfLibrarySearchField::Camera)
                values = cameraValues;
        else if (definition.field == NfCore::NfLibrarySearchField::Lens)
                values = lensValues;
        value->addItems(values);
        value->setCurrentIndex(-1);
        if (value->isEditable())
                value->setEditText(QString{});
        layout->addWidget(value);
        return container;
}

} // namespace

NfSearchCriteriaWidget::NfSearchCriteriaWidget(const QStringList& cameraValues,
                                               const QStringList& lensValues,
                                               QWidget* parent)
        : NfStyledWidget(parent)
        , m_cameraValues{cameraValues}
        , m_lensValues{lensValues}
{
        setObjectName("SearchCriteriaWidget");
        setMinimumWidth(0);
        setMaximumWidth(680);
        resize(640, sizeHint().height());

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(16, 14, 16, 14);
        layout->setSpacing(10);

        auto* criteriaLabel = new QLabel(tr("Criteria"), this);
        criteriaLabel->setObjectName("SearchCriteriaHeading");
        layout->addWidget(criteriaLabel);

        m_criteriaList = new QWidget(this);
        m_criteriaLayout = new QVBoxLayout(m_criteriaList);
        m_criteriaLayout->setContentsMargins(0, 0, 0, 0);
        m_criteriaLayout->setSpacing(6);
        layout->addWidget(m_criteriaList);

        auto* footer = new QHBoxLayout();
        footer->setSpacing(8);
        footer->addStretch();

        auto* clearButton = new QPushButton(tr("Clear"), this);
        clearButton->setObjectName("ClearSearchCriteriaButton");
        footer->addWidget(clearButton);

        auto* searchButton = new QPushButton(tr("Search"), this);
        searchButton->setObjectName("SubmitSearchCriteriaButton");
        searchButton->setDefault(true);
        footer->addWidget(searchButton);
        layout->addLayout(footer);

        QObject::connect(searchButton,
                         &QPushButton::clicked,
                         this,
                         [this]() {
                                 emit searchRequested();
                                 close();
                         });
        QObject::connect(clearButton,
                         &QPushButton::clicked,
                         this,
                         [this]() {
                                 for (const auto& row : m_criteria) {
                                         auto* valueBox =
                                                 row.widget->findChild<QComboBox*>(
                                                         "SearchCriterionInput");
                                         if (valueBox) {
                                                 valueBox->setCurrentIndex(-1);
                                                 if (valueBox->isEditable())
                                                         valueBox->setEditText(QString{});
                                         }
                                         const auto dateEdits =
                                                 row.widget->findChildren<QDateEdit*>();
                                         for (auto* dateEdit : dateEdits)
                                                 dateEdit->setDate(dateEdit->minimumDate());
                                 }
                         });

        addCriterion(tr("Date Taken"));
        addCriterion(tr("Camera"));
        addCriterion(tr("Lens"));
        addCriterion(tr("File Format"));
        addCriterion(tr("Folder"));
}

void NfSearchCriteriaWidget::addCriterion(const QString& name)
{
        const auto* definition = definitionFor(name);
        if (!definition)
                return;

        auto* row = new QWidget(m_criteriaList);
        row->setObjectName("SearchCriterionRow");
        auto* layout = new QHBoxLayout(row);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(8);

        auto* nameLabel = new QLabel(name, row);
        nameLabel->setMinimumWidth(112);
        nameLabel->setObjectName("SearchCriterionName");
        layout->addWidget(nameLabel);

        auto* operatorBox = new QComboBox(row);
        operatorBox->setObjectName("SearchCriterionOperator");
        operatorBox->addItems(definition->operators);
        if (name == tr("Date Taken"))
                operatorBox->setCurrentText(tr("Between"));
        operatorBox->setMinimumWidth(112);
        layout->addWidget(operatorBox);

        auto* valueHost = new QWidget(row);
        valueHost->setObjectName("SearchCriterionValue");
        auto* valueHostLayout = new QVBoxLayout(valueHost);
        valueHostLayout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(valueHost, 1);
        valueHostLayout->addWidget(
                createValueControl(*definition,
                                   operatorBox->currentText(),
                                   m_cameraValues,
                                   m_lensValues,
                                   valueHost));

        m_criteria.append({definition->field, row});
        m_criteriaLayout->addWidget(row);

        QObject::connect(operatorBox,
                         &QComboBox::currentTextChanged,
                         valueHost,
                         [definition,
                          valueHostLayout,
                          valueHost,
                          cameraValues = m_cameraValues,
                          lensValues = m_lensValues](const QString& op) {
                                 auto* oldControl = valueHostLayout->takeAt(0)->widget();
                                 delete oldControl;
                                 valueHostLayout->addWidget(
                                         createValueControl(*definition,
                                                            op,
                                                            cameraValues,
                                                            lensValues,
                                                            valueHost));
                         });
}

std::vector<NfCore::NfLibrarySearchCriterion>
NfSearchCriteriaWidget::criteria() const
{
        using NfCore::NfLibrarySearchCriterion;
        using NfCore::NfLibrarySearchOperator;

        auto result = std::vector<NfLibrarySearchCriterion>{};
        result.reserve(static_cast<size_t>(m_criteria.size()));
        for (const auto& row : m_criteria) {
                const auto* definition = row.widget->findChild<QComboBox*>(
                        "SearchCriterionOperator");
                if (!definition)
                        continue;

                auto op = NfLibrarySearchOperator::Is;
                const auto operatorText = definition->currentText();
                if (operatorText == tr("Contains"))
                        op = NfLibrarySearchOperator::Contains;
                else if (operatorText == tr("Before"))
                        op = NfLibrarySearchOperator::Before;
                else if (operatorText == tr("After"))
                        op = NfLibrarySearchOperator::After;
                else if (operatorText == tr("Between"))
                        op = NfLibrarySearchOperator::Between;

                auto criterion = NfLibrarySearchCriterion{
                        .field = row.field,
                        .op = op
                };
                const auto* valueBox = row.widget->findChild<QComboBox*>(
                        "SearchCriterionInput");
                if (valueBox) {
                        criterion.value = valueBox->currentText().toStdString();
                        if (criterion.value.empty())
                                continue;
                } else {
                        const auto dateEdits =
                                row.widget->findChildren<QDateEdit*>();
                        if (dateEdits.isEmpty()
                            || dateEdits.first()->date()
                                       == dateEdits.first()->minimumDate())
                                continue;
                        criterion.value = dateEdits.first()->date()
                                                          .toString(Qt::ISODate)
                                                          .toStdString();
                        if (op == NfLibrarySearchOperator::Between) {
                                if (dateEdits.size() < 2
                                    || dateEdits.at(1)->date()
                                               == dateEdits.at(1)->minimumDate())
                                        continue;
                                criterion.secondValue =
                                        dateEdits.at(1)->date()
                                                                .toString(Qt::ISODate)
                                                                .toStdString();
                        }
                }
                result.push_back(criterion);
        }
        return result;
}

void NfSearchCriteriaWidget::setEquipmentValues(const QStringList& cameraValues,
                                               const QStringList& lensValues)
{
        m_cameraValues = cameraValues;
        m_lensValues = lensValues;
        for (const auto& row : m_criteria) {
                if (row.field != NfCore::NfLibrarySearchField::Camera
                    && row.field != NfCore::NfLibrarySearchField::Lens)
                        continue;

                auto* valueBox = row.widget->findChild<QComboBox*>(
                        "SearchCriterionInput");
                if (!valueBox)
                        continue;

                const auto currentValue = valueBox->currentText();
                valueBox->clear();
                valueBox->addItems(row.field == NfCore::NfLibrarySearchField::Camera
                                           ? m_cameraValues
                                           : m_lensValues);
                if (currentValue.isEmpty())
                        valueBox->setCurrentIndex(-1);
                else
                        valueBox->setCurrentText(currentValue);
        }
}

} // namespace NfDesktop
