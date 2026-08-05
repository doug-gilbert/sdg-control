
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

    formLayout = new QFormLayout(groupBox);

#ifdef SDG_DEVELOPER_UI
    statusLabel = new QLabel(
        QString("CH%1: --").arg(channel),
        groupBox);

    statusLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard);

    formLayout->addRow("Status:", statusLabel);
#endif

    outputCheck = new QCheckBox(
        QString("CH%1 Output").arg(channel),
        groupBox);

    waveformCombo = new QComboBox(groupBox);

    waveformCombo->addItems({
        "SINE",
        "SQUARE",
        "RAMP",
        "PULSE",
        "NOISE",
        "ARB"
    });

    frequencySpin = new QDoubleSpinBox(groupBox);
    frequencySpin->setRange(0.000'01, 120'000'000);
    frequencySpin->setDecimals(6);
    frequencySpin->setSingleStep(0.000'01);
    frequencySpin->setSuffix(" Hz");
    frequencySpin->setKeyboardTracking(false);

    amplitudeSpin = new QDoubleSpinBox(groupBox);
    amplitudeSpin->setRange(0, 20);
    amplitudeSpin->setDecimals(3);
    amplitudeSpin->setSingleStep(0.1);
    amplitudeSpin->setSuffix(" V");
    amplitudeSpin->setKeyboardTracking(false);

    offsetSpin = new QDoubleSpinBox(groupBox);
    offsetSpin->setRange(-10, 10);
    offsetSpin->setDecimals(3);
    offsetSpin->setSingleStep(0.1);
    offsetSpin->setSuffix(" V");
    offsetSpin->setKeyboardTracking(false);

    phaseSpin = new QDoubleSpinBox(groupBox);
    phaseSpin->setRange(-360.0, 360.0);
    phaseSpin->setDecimals(1);
    phaseSpin->setSingleStep(1.0);
    phaseSpin->setSuffix("°");
    phaseSpin->setKeyboardTracking(false);

    symmetrySpin = new QDoubleSpinBox(groupBox);
    symmetrySpin->setRange(0.0, 100.0);
    symmetrySpin->setDecimals(1);
    symmetrySpin->setSingleStep(1.0);
    symmetrySpin->setSuffix(" %");
    symmetrySpin->setKeyboardTracking(false);

    waveformLabel = new QLabel("Waveform:", groupBox);
    frequencyLabel = new QLabel("Frequency:", groupBox);
    amplitudeLabel = new QLabel("Amplitude:", groupBox);
    offsetLabel = new QLabel("Offset:", groupBox);
    phaseLabel = new QLabel("Phase:", groupBox);
    symmetryLabel = new QLabel("Symmetry:", groupBox);
    // Expect this call will hide Symmetry field if waveform != RAMP
    updateControlVisibility();

    formLayout->addRow(waveformLabel, waveformCombo);
    formLayout->addRow(frequencyLabel, frequencySpin);
    formLayout->addRow(amplitudeLabel, amplitudeSpin);
    formLayout->addRow(offsetLabel, offsetSpin);
    formLayout->addRow(phaseLabel, phaseSpin);
    formLayout->addRow(symmetryLabel, symmetrySpin);
    formLayout->addRow(outputCheck);

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

    connect(symmetrySpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit symmetryChanged(this->channel, value);
            });

    connect(waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                updateControlVisibility();
                emit waveformChanged(this->channel, waveform);
            });
}

void ChannelWidget::setFrequency(double frequency)
{
    frequencySpin->blockSignals(true);
    frequencySpin->setValue(frequency);
    frequencySpin->blockSignals(false);
}

void ChannelWidget::setAmplitude(double amplitude)
{
    amplitudeSpin->blockSignals(true);
    amplitudeSpin->setValue(amplitude);
    amplitudeSpin->blockSignals(false);
}

void ChannelWidget::setOffset(double offset)
{
    offsetSpin->blockSignals(true);
    offsetSpin->setValue(offset);
    offsetSpin->blockSignals(false);
}

void ChannelWidget::setSymmetry(double percent)
{
    symmetrySpin->blockSignals(true);
    symmetrySpin->setValue(percent);
    symmetrySpin->blockSignals(false);
}

void ChannelWidget::setOutput(bool enabled)
{
    outputCheck->blockSignals(true);
    outputCheck->setChecked(enabled);
    outputCheck->blockSignals(false);
}

void ChannelWidget::setStatus(const QString &text)
{
#ifdef SDG_DEVELOPER_UI
    statusLabel->setText(text);
#else
    Q_UNUSED(text);
#endif
}

void ChannelWidget::setWaveform(const QString &waveform)
{
    waveformCombo->blockSignals(true);
    waveformCombo->setCurrentText(waveform);
    waveformCombo->blockSignals(false);
    updateControlVisibility();
}

void ChannelWidget::setPhase(double value)
{
    phaseSpin->blockSignals(true);
    phaseSpin->setValue(value);
    phaseSpin->blockSignals(false);
}

void ChannelWidget::updateControlVisibility()
{
    bool showSymmetry =
        (waveformCombo->currentText() == "RAMP");

    symmetryLabel->setVisible(showSymmetry);
    symmetrySpin->setVisible(showSymmetry);
}
