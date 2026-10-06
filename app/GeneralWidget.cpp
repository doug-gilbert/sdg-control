/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>
#include <QApplication>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "GeneralWidget.h"
#include "GeneralState.h"
#include "AppController.h"
#include "AppHelper.h"
#include "Utility.h"
#include "debug.h"


GeneralWidget::GeneralWidget(AppController *controller,
                             const GeneralDirtyState * dirtyState,
                             QWidget *parent)
              : m_controller(controller),
                m_dirtyState(dirtyState),
                QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);

    auto *headerLayout = new QHBoxLayout;

    const QString labelStr("General Settings");
    auto *titleLabel = new QLabel(labelStr, this);

    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);

    m_closeButton = new QPushButton("x", this);
    m_closeButton->setFixedSize(28, 28);
    m_closeButton->setToolTip("Hide General Settings");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_closeButton);

    outerLayout->addLayout(headerLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);

    m_scrollContents = new QWidget;
    m_scrollArea->setWidget(m_scrollContents);

    outerLayout->addWidget(m_scrollArea);

    auto *scrollLayout = new QVBoxLayout(m_scrollContents);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSizeConstraint(QLayout::SetMinimumSize);

    m_groupBox = new QGroupBox(m_scrollContents);
    m_groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_formLayout = new QFormLayout(m_groupBox);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    scrollLayout->addWidget(m_groupBox);

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    m_clockSourceCombo = new QComboBox(m_groupBox);
    m_clockSourceCombo->setObjectName("clockSourceCombo");
    m_clockSourceCombo->addItem(tr("Internal"),
                                   static_cast<int>(ClockSource::Internal));
    m_clockSourceCombo->addItem(tr("External"),
                                   static_cast<int>(ClockSource::External));
    m_clockSourceCombo->setCurrentIndex(0);

    m_overVoltageProtectionCombo = new QComboBox(m_groupBox);
    m_overVoltageProtectionCombo->setObjectName("overVoltageProtectionCombo");
    m_overVoltageProtectionCombo->addItem(tr("Off"),
                               static_cast<int>(OverVoltageProtection::Off));
    m_overVoltageProtectionCombo->addItem(tr("On"),
                               static_cast<int>(OverVoltageProtection::On));
    m_overVoltageProtectionCombo->setCurrentIndex(0);

    m_formLayout->addRow(tr("Clock source:"), m_clockSourceCombo);
    m_formLayout->addRow(tr("Over voltage protection:"),
                         m_overVoltageProtectionCombo);

    connect(m_clockSourceCombo, &QComboBox::currentIndexChanged,
            this,
            [this](int index)
            {
                const auto source = static_cast<ClockSource>(
                    m_clockSourceCombo->itemData(index).toInt());

                emit clockSourceChanged(source);
            });

    connect(m_overVoltageProtectionCombo, &QComboBox::currentIndexChanged,
            this,
            [this](int index)
            {
                const auto src = static_cast<OverVoltageProtection>(
                    m_overVoltageProtectionCombo->itemData(index).toInt());

                emit overVoltageProtectionChanged(src);
            });

    connect(m_closeButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                DEBUG_FUNC << "Close clicked";
                emit hideRequested();
            });

    hide();
}

void GeneralWidget::setUiClockSource(ClockSource cs)
{
    m_clockSourceCombo->blockSignals(true);
    m_clockSourceCombo->setCurrentText(Utility::clockSourceToString(cs));
    m_clockSourceCombo->blockSignals(false);
}

void GeneralWidget::setUiOverVoltageProtection(OverVoltageProtection ovp)
{
    m_overVoltageProtectionCombo->blockSignals(true);
    m_overVoltageProtectionCombo->setCurrentText(
                  Utility::overVoltageProtectionToString(ovp));
    m_overVoltageProtectionCombo->blockSignals(false);

}

void GeneralWidget::selectAllIfDirty(bool enabled)
{
    DEBUG_FUNC;

    if (enabled) {
        if (m_dirtyState->m_clockSource)
            AppHelper::selectCombo(m_clockSourceCombo);
        if (m_dirtyState->m_overVoltageProtection)
            AppHelper::selectCombo(m_overVoltageProtectionCombo);
    } else {
        AppHelper::deselectCombo(m_clockSourceCombo);
        AppHelper::deselectCombo(m_overVoltageProtectionCombo);
    }
}
