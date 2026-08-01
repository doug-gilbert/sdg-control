#include "ChannelWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>

ChannelWidget::ChannelWidget(int channel, QWidget *parent)
    : QWidget(parent),
      channel(channel)
{
    auto *layout = new QVBoxLayout(this);

    label = new QLabel(
        QString("CH%1: --").arg(channel),
        this);

    outputCheck = new QCheckBox(
        QString("CH%1 Output").arg(channel),
        this);

    frequencySpin = new QDoubleSpinBox(this);
    frequencySpin->setRange(0.001, 120'000'000);
    frequencySpin->setDecimals(3);
    frequencySpin->setSingleStep(1.0);
    frequencySpin->setSuffix(" Hz");

    amplitudeSpin = new QDoubleSpinBox(this);
    amplitudeSpin->setRange(0, 20);
    amplitudeSpin->setDecimals(3);
    amplitudeSpin->setSingleStep(0.1);
    amplitudeSpin->setSuffix(" V");

    offsetSpin = new QDoubleSpinBox(this);
    offsetSpin->setRange(-10, 10);
    offsetSpin->setDecimals(3);
    offsetSpin->setSingleStep(0.1);
    offsetSpin->setSuffix(" V");

    layout->addWidget(label);
    layout->addWidget(frequencySpin);
    layout->addWidget(amplitudeSpin);
    layout->addWidget(offsetSpin);
    layout->addWidget(outputCheck);
}

void ChannelWidget::setFrequency(double value)
{
    frequencySpin->blockSignals(true);
    frequencySpin->setValue(value);
    frequencySpin->blockSignals(false);
}

void ChannelWidget::setAmplitude(double value)
{
    amplitudeSpin->blockSignals(true);
    amplitudeSpin->setValue(value);
    amplitudeSpin->blockSignals(false);
}

void ChannelWidget::setOffset(double value)
{
    offsetSpin->blockSignals(true);
    offsetSpin->setValue(value);
    blockSignals(false);
}

void ChannelWidget::setOutput(bool enabled)
{
    outputCheck->blockSignals(true);
    outputCheck->setChecked(enabled);
    outputCheck->blockSignals(false);
}

void ChannelWidget::setStatus(const QString &text)
{
    label->setText(text);
}
