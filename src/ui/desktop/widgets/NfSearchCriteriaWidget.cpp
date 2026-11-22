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
#include "NfDesktopContext.h"
#include "NfLibraryTreeModel.h"
#include "NfUiLibraryModeState.h"
#include "NfUiState.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <chrono>
#include <vector>

namespace NfDesktop {

namespace {

struct CriterionDefinition
{
        NfCore::NfLibrarySearchField field;
        QStringList operators;
        QStringList values;
        bool editable{false};
        bool date{false};
};

const CriterionDefinition* definitionFor(const QString& name)
{
        static const auto definitions = [] {
                QHash<QString, CriterionDefinition> result;
                result.insert(QObject::tr("Camera"),
                              CriterionDefinition{NfCore::NfLibrarySearchField::Camera,
                                                  {QObject::tr("Is"), QObject::tr("Contains")},
                                                  {},
                                                  true,
                                                  false});
                result.insert(QObject::tr("Lens"),
                              CriterionDefinition{NfCore::NfLibrarySearchField::Lens,
                                                  {QObject::tr("Is"), QObject::tr("Contains")},
                                                  {},
                                                 true,
                                                 false});
                result.insert(QObject::tr("Date Taken"),
                              CriterionDefinition{NfCore::NfLibrarySearchField::DateTaken,
                                                  {QObject::tr("Is"), QObject::tr("Before"), QObject::tr("After"), QObject::tr("Between")},
                                                  {},
                                                  false,
                                                  true});
                result.insert(QObject::tr("File Format"),
                              CriterionDefinition{NfCore::NfLibrarySearchField::FileFormat,
                                                  {QObject::tr("Is")},
                                                  {QObject::tr("RAW"), QObject::tr("JPEG"), QObject::tr("PNG")},
                                                  false,
                                                  false});
                result.insert(QObject::tr("Folder"),
                              CriterionDefinition{NfCore::NfLibrarySearchField::Folder,
                                                 {QObject::tr("Is"), QObject::tr("Contains")},
                                                 {},
                                                 true,
                                                 false});
                return result;
        }();

        const auto res = definitions.constFind(name);
        return res != definitions.cend() ? &res.value() : nullptr;
}

QWidget* createValueControl(const CriterionDefinition& definition,
                            const QString& op,
                            const NfLibraryTreeModel::Equipment& equipment,
                            const std::optional<NfCore::NfDateRange>& dateTimeRange,
                            QWidget* parent)
{
        auto* container = new QWidget(parent);
        container->setObjectName("SearchCriterionValueControl");
        auto* layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(8);

        const auto addDateEdit = [layout, container, dateTimeRange]() {
                const auto today = QDate::currentDate();
                auto minimumDate = today.addYears(-10);
                auto maximumDate = today;
                if (dateTimeRange
                    && dateTimeRange->end_ticks > dateTimeRange->start_ticks) {
                        const auto toDate = [](int64_t ticks) {
                                const auto timePoint =
                                        std::chrono::system_clock::time_point{
                                                std::chrono::system_clock::duration{ticks}};
                                const auto localTime = std::chrono::zoned_time{
                                        std::chrono::current_zone(),
                                        timePoint}.get_local_time();
                                const auto day =
                                        std::chrono::floor<std::chrono::days>(localTime);
                                const std::chrono::year_month_day ymd{day};
                                return QDate{static_cast<int>(ymd.year()),
                                             static_cast<int>(unsigned(ymd.month())),
                                             static_cast<int>(unsigned(ymd.day()))};
                        };

                        const auto libraryMinimum = toDate(dateTimeRange->start_ticks);
                        const auto libraryMaximum = toDate(dateTimeRange->end_ticks - 1);
                        if (libraryMinimum.isValid() && libraryMaximum.isValid()
                            && libraryMinimum <= libraryMaximum) {
                                minimumDate = libraryMinimum;
                                maximumDate = libraryMaximum;
                        }
                }
                const auto unsetDate = minimumDate.addDays(-1);
                auto* edit = new QDateEdit(container);
                edit->setCalendarPopup(true);
                edit->setDisplayFormat("yyyy-MM-dd");
                edit->setMinimumDate(unsetDate);
                edit->setMaximumDate(maximumDate);
                edit->setSpecialValueText(QObject::tr("Select date"));
                edit->setDate(unsetDate);
                const auto initialDate = today < minimumDate
                        ? minimumDate
                        : today > maximumDate ? maximumDate : today;
                edit->calendarWidget()->setCurrentPage(initialDate.year(),
                                                      initialDate.month());
                edit->setMinimumWidth(128);
                layout->addWidget(edit);
        };

        if (definition.date) {
                addDateEdit();
                if (op == QObject::tr("Between"))
                        addDateEdit();
                return container;
        }

        auto* valueComboBox = new QComboBox(container);

        using EquipmentInfo = NfLibraryTreeModel::EquipmentInfo;
        auto addEquipmentValues = [valueComboBox] (const std::vector<EquipmentInfo>& info) {
                for (const auto& el: info)
                        valueComboBox->addItem(el.name,
                                               static_cast<qlonglong>(el.id));
        };

        valueComboBox->setObjectName("SearchCriterionInput");
        valueComboBox->setMinimumWidth(220);
        valueComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        if (definition.editable)
                valueComboBox->setEditable(true);
        if (definition.field == NfCore::NfLibrarySearchField::Camera)
                addEquipmentValues(equipment.cameras);
        else if (definition.field == NfCore::NfLibrarySearchField::Lens)
                addEquipmentValues(equipment.lenses);
        else
                valueComboBox->addItems(definition.values);

        valueComboBox->setCurrentIndex(-1);
        if (valueComboBox->isEditable())
                valueComboBox->setEditText(QString{});
        layout->addWidget(valueComboBox);

        return container;
}

} // namespace

        NfSearchCriteriaWidget::NfSearchCriteriaWidget(const NfDesktopContext &ctx,
                                                       QWidget* parent)
        : NfStyledWidget(parent)
        , m_context{ctx}
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

        m_criteriaListWidget = new QWidget(this);
        m_criteriaListWidget->setObjectName("CriteriaListWidget");
        m_criteriaLayout = new QVBoxLayout(m_criteriaListWidget);
        m_criteriaLayout->setContentsMargins(0, 0, 0, 0);
        m_criteriaLayout->setSpacing(6);
        layout->addWidget(m_criteriaListWidget);

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
                         &NfSearchCriteriaWidget::clearCriteria);

        addCriterion(tr("Date Taken"));
        addCriterion(tr("Camera"));
        addCriterion(tr("Lens"));
        addCriterion(tr("File Format"));
        addCriterion(tr("Folder"));
}

void NfSearchCriteriaWidget::clearCriteria()
{
        for (const auto& row : m_criteria) {
                auto* valueBox = row.widget->findChild<QComboBox*>("SearchCriterionInput");
                if (valueBox) {
                        valueBox->setCurrentIndex(-1);
                        if (valueBox->isEditable())
                                valueBox->setEditText(QString{});
                }

                const auto dateEdits = row.widget->findChildren<QDateEdit*>();
                for (auto* dateEdit : dateEdits) {
                        dateEdit->setDate(dateEdit->minimumDate());
                        const auto today = QDate::currentDate();
                        dateEdit->calendarWidget()->setCurrentPage(today.year(), today.month());
                }
        }
}

void NfSearchCriteriaWidget::addCriterion(const QString& name)
{
        const auto* definition = definitionFor(name);
        if (!definition)
                return;

        auto* row = new QWidget(m_criteriaListWidget);
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

        auto libraryId = m_context.uiState()->libraryModeState()->currentLibrary();
        auto equipment = m_context.libraryTreeModel->getEquipment(libraryId);
        auto dateTimeRange = m_context.libraryTreeModel->getDateTimeRange(libraryId);
        valueHostLayout->addWidget(createValueControl(*definition,
                                                      operatorBox->currentText(),
                                                      equipment,
                                                      dateTimeRange,
                                                      valueHost));

        m_criteria.push_back({definition->field, row});
        m_criteriaLayout->addWidget(row);

        QObject::connect(operatorBox,
                         &QComboBox::currentTextChanged,
                         valueHost,
                         [definition,
                          valueHostLayout,
                          valueHost,
                          equipment,
                          dateTimeRange](const QString& op) {
                                 auto* oldControl = valueHostLayout->takeAt(0)->widget();
                                 delete oldControl;
                                 valueHostLayout->addWidget(createValueControl(*definition,
                                                                               op,
                                                                               equipment,
                                                                               dateTimeRange,
                                                                               valueHost));
                         });
}

std::vector<NfCore::NfLibraryQueryCriterion>
NfSearchCriteriaWidget::criteria() const
{
        using NfCore::NfLibraryQueryCriterion;
        using NfCore::NfLibrarySearchOperator;

        auto result = std::vector<NfLibraryQueryCriterion>{};
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

                auto criterion = NfLibraryQueryCriterion{
                        .field = row.field,
                        .op = op
                };
                const auto* valueBox = row.widget->findChild<QComboBox*>(
                        "SearchCriterionInput");
                if (valueBox) {
                        const auto value = valueBox->currentText().toStdString();
                        if (value.empty())
                                continue;
                        criterion.value = value;
                } else {
                        const auto dateEdits =
                                row.widget->findChildren<QDateEdit*>();
                        if (dateEdits.isEmpty()
                            || dateEdits.first()->date()
                                       == dateEdits.first()->minimumDate())
                                continue;
                        const auto startDate = dateEdits.first()->date();
                        const auto startRange = NfCore::NfTimeUtils::getDayRange(
                                std::chrono::year{startDate.year()}
                                / std::chrono::month{static_cast<unsigned>(startDate.month())}
                                / std::chrono::day{static_cast<unsigned>(startDate.day())});
                        if (op == NfLibrarySearchOperator::Between) {
                                if (dateEdits.size() < 2
                                    || dateEdits.at(1)->date()
                                               == dateEdits.at(1)->minimumDate())
                                        continue;
                                const auto endDate = dateEdits.at(1)->date();
                                const auto endRange = NfCore::NfTimeUtils::getDayRange(
                                        std::chrono::year{endDate.year()}
                                        / std::chrono::month{static_cast<unsigned>(endDate.month())}
                                        / std::chrono::day{static_cast<unsigned>(endDate.day())});
                                criterion.value = NfCore::NfDateRange{
                                        startRange.start_ticks, endRange.end_ticks};
                        } else {
                                criterion.value = startRange;
                        }
                }
                result.push_back(criterion);
        }

        return result;
}

} // namespace NfDesktop
