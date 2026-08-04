
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "debug.h"
#include "ChannelWidget.h"

ChannelWidget::ChannelWidget(int my_channel, QWidget *parent)
    : QWidget(parent),
      channel(my_channel)
{
    auto *outerLayout = new QVBoxLayout(this);

    groupBox = new QGroupBox(
        QString("Channel %1").arg(channel),
        this);

    auto *layout = new QFormLayout(groupBox);

    label = new QLabel(
        QString("CH%1: --").arg(channel),
        this);

    outputCheck = new QCheckBox(
        QString("CH%1 Output").arg(channel),
        this);

    waveformCombo = new QComboBox(this);

    waveformCombo->addItems({
        "SINE",
        "SQUARE",
        "RAMP",
        "PULSE",
        "NOISE",
        "ARB"
    });

    frequencySpin = new QDoubleSpinBox(this);
    frequencySpin->setRange(0.000'01, 120'000'000);
    frequencySpin->setDecimals(6);
    frequencySpin->setSingleStep(0.000'01);
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

    phaseSpin = new QDoubleSpinBox(this);
    phaseSpin->setRange(-360.0, 360.0);
    phaseSpin->setDecimals(1);
    phaseSpin->setSingleStep(1.0);
    phaseSpin->setSuffix("°");

    layout->addRow(label);

    layout->addRow("Waveform:", waveformCombo);
    layout->addRow("Frequency:", frequencySpin);
    layout->addRow("Amplitude:", amplitudeSpin);
    layout->addRow("Offset:", offsetSpin);
    layout->addRow("Phase:", phaseSpin);
    layout->addRow(outputCheck);

    outerLayout->addWidget(groupBox);

    connect(outputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit outputChanged(this->channel, enabled);
            });

    connect(frequencySpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit frequencyChanged(this->channel, value);
            });

    connect(amplitudeSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit amplitudeChanged(this->channel, value);
            });

    connect(offsetSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit offsetChanged(this->channel, value);
            });

    connect(phaseSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit phaseChanged(this->channel, value);
            });

    connect(waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                emit waveformChanged(this->channel, waveform);
            });
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
    offsetSpin->blockSignals(false);
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

void ChannelWidget::setWaveform(const QString &waveform)
{
    waveformCombo->blockSignals(true);
    waveformCombo->setCurrentText(waveform);
    waveformCombo->blockSignals(false);
}

void ChannelWidget::setPhase(double value)
{
    phaseSpin->blockSignals(true);
    phaseSpin->setValue(value);
    phaseSpin->blockSignals(false);
}
