/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QCheckBox;

namespace AppHelper
{
    void selectCombo(QComboBox *combo);
    void deselectCombo(QComboBox *combo);

    void selectSpinBox(QDoubleSpinBox *sBox);
    void deselectSpinBox(QDoubleSpinBox *sBox);

    void selectLabel(QLabel *label);
    void deselectLabel(QLabel *label);

    void selectCheckBox(QCheckBox *cb);
    void deselectCheckBox(QCheckBox *cb);
}
