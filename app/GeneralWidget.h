
#pragma once

#include <QWidget>

#include "SettingsIO.h"

class QPushButton;
class QScrollArea;
class QComboBox;
class QGroupBox;
class QFormLayout;

class AppController;


class GeneralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralWidget(AppController *controller,
                           const GeneralDirtyState *dirtyState,
                           QWidget *parent = nullptr);

    // Going from internal state (where Units may be normalized) to UI
    void setUiClockSource(ClockSource cs);
    void setUiOverVoltageProtection(OverVoltageProtection ovp);

    // If enabled is true, the contents of each field is selected
    // (highlighted) if the corresponding dirty flag is set. For QuantityEdit
    // based numeric fields, only the the numeric part is selected; for
    // ComboBox only fields, the "dirty" selection is highlighted. If enabled
    // is false, and if the corresponding dirty flag is set, then the
    // contents of each field is deselected. So selectAllIfDirty(false)
    // undoes what selectAllIfDirty(true) does.
    void selectAllIfDirty(bool enabled);

signals:
    void clockSourceChanged(ClockSource newSource);

    void overVoltageProtectionChanged(OverVoltageProtection newOVP);

    void hideRequested();

private:
    AppController *m_controller = nullptr;

    const GeneralDirtyState *m_dirtyState;

    QPushButton *m_closeButton = nullptr;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContents = nullptr;

    QGroupBox *m_groupBox;
    QFormLayout *m_formLayout;

    QComboBox *m_clockSourceCombo = nullptr;
    QComboBox *m_overVoltageProtectionCombo = nullptr;
};
