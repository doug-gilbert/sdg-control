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
    QLabel *ch2Label;
    QCheckBox *ch1OutputCheck;
    QCheckBox *ch2OutputCheck;
    QDoubleSpinBox *ch1FrequencySpin;
    QDoubleSpinBox *ch1AmplitudeSpin;
    QDoubleSpinBox *ch1OffsetSpin;

    SDG2000X generator;
};
