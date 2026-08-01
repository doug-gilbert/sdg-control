#pragma once

#include <QWidget>

class QLabel;
class QCheckBox;
class QDoubleSpinBox;

class ChannelWidget : public QWidget
{
public:
    explicit ChannelWidget(int channel, QWidget *parent = nullptr);

private:
    int channel;

    QLabel *label;
    QCheckBox *outputCheck;

    QDoubleSpinBox *frequencySpin;
    QDoubleSpinBox *amplitudeSpin;
    QDoubleSpinBox *offsetSpin;
};
