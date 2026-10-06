/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "AppHelper.h"
#include "debug.h"


void AppHelper::selectCombo(QComboBox *combo)
{
    DEBUG_FUNC << " Object:" << combo->objectName();

    combo->setStyleSheet(
        "QComboBox {"
        " background-color: palette(highlight);"
        " color: palette(highlighted-text);"
        "}"
    );
}

void AppHelper::deselectCombo(QComboBox *combo)
{
    combo->setStyleSheet({});
}

// Highlight all characters in a field
void AppHelper::selectSpinBox(QDoubleSpinBox *sBox)
{
    // Would prefer to use: 'sBox->lineEdit()->selectAll(); ' but lineEdit()
    // is protected.
    sBox->setStyleSheet(
        "QDoubleSpinBox {"
        " background-color: palette(highlight);"
        " color: palette(highlighted-text);"
        "}"
    );
}

void AppHelper::deselectSpinBox(QDoubleSpinBox *sBox)
{
    sBox->setStyleSheet({});
}

void AppHelper::selectLabel(QLabel *label)
{
    label->setStyleSheet(
        "QLabel {"
        " background-color: palette(highlight);"
        " color: palette(highlighted-text);"
        "}"
    );
}

void AppHelper::deselectLabel(QLabel *label)
{
    label->setStyleSheet({});
}

void AppHelper::selectCheckBox(QCheckBox *cb)
{
    cb->setStyleSheet(
        "QCheckBox {"
        " background-color: palette(highlight);"
        " color: palette(highlighted-text);"
        "}"
    );
}

void AppHelper::deselectCheckBox(QCheckBox *cb)
{
    cb->setStyleSheet({});
}
