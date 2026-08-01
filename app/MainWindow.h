#pragma once

#include <QMainWindow>
#include <QDoubleSpinBox>

#include "SDG2000X.h"

class QLabel;
class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshClicked();

private:
    QLabel *idLabel;

    QLabel *ch1Label;
    QCheckBox *ch1OutputCheck;
    QDoubleSpinBox *ch1FrequencySpin;
    QDoubleSpinBox *ch1AmplitudeSpin;
    QDoubleSpinBox *ch1OffsetSpin;

    QLabel *ch2Label;
    QCheckBox *ch2OutputCheck;
    QDoubleSpinBox *ch2FrequencySpin;
    QDoubleSpinBox *ch2AmplitudeSpin;
    QDoubleSpinBox *ch2OffsetSpin;

    SDG2000X generator;
};
