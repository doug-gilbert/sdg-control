
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
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

    statusLabel->setWordWrap(true);
    statusLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::MinimumExpanding);
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

    rampSymmetrySpin = new QDoubleSpinBox(groupBox);
    rampSymmetrySpin->setRange(0.0, 100.0);
    rampSymmetrySpin->setDecimals(1);
    rampSymmetrySpin->setSingleStep(1.0);
    rampSymmetrySpin->setSuffix(" %");
    rampSymmetrySpin->setKeyboardTracking(false);

    pulseWidthSpin = new QDoubleSpinBox(groupBox);
    pulseWidthSpin->setRange(0.000'000'001, 1.0);
    pulseWidthSpin->setDecimals(9);
    pulseWidthSpin->setSingleStep(0.000'001);
    pulseWidthSpin->setSuffix(" s");
    pulseWidthSpin->setKeyboardTracking(false);

    pulseRiseSpin = new QDoubleSpinBox(groupBox);
    pulseRiseSpin->setRange(0.001, 1'000'000.0);
    pulseRiseSpin->setDecimals(3);
    pulseRiseSpin->setSingleStep(0.1);
    pulseRiseSpin->setSuffix(" ns");
    pulseRiseSpin->setKeyboardTracking(false);

    pulseFallSpin = new QDoubleSpinBox(groupBox);
    pulseFallSpin->setRange(0.001, 1'000'000.0);
    pulseFallSpin->setDecimals(3);
    pulseFallSpin->setSingleStep(0.1);
    pulseFallSpin->setSuffix(" ns");
    pulseFallSpin->setKeyboardTracking(false);

    pulseDutySpin = new QDoubleSpinBox(groupBox);
    pulseDutySpin->setRange(0.0, 100.0);
    pulseDutySpin->setDecimals(3);
    pulseDutySpin->setSuffix(" %");
    pulseDutySpin->setReadOnly(true);
    pulseDutySpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    pulseDutySpin->setFocusPolicy(Qt::NoFocus);

    waveformLabel = new QLabel("Waveform:", groupBox);
    frequencyLabel = new QLabel("Frequency:", groupBox);
    amplitudeLabel = new QLabel("Amplitude:", groupBox);
    offsetLabel = new QLabel("Offset:", groupBox);
    phaseLabel = new QLabel("Phase:", groupBox);
    rampSymmetryLabel = new QLabel("Ramp symmetry:", groupBox);
    pulseWidthLabel = new QLabel("Pulse Width:", groupBox);
    pulseRiseLabel = new QLabel("Pulse Rise:", groupBox);
    pulseFallLabel = new QLabel("Pulse Fall:", groupBox);
    pulseDutyLabel = new QLabel("Pulse Duty:", groupBox);

    updateControlVisibility();

    formLayout->addRow(waveformLabel, waveformCombo);
    formLayout->addRow(frequencyLabel, frequencySpin);
    formLayout->addRow(amplitudeLabel, amplitudeSpin);
    formLayout->addRow(offsetLabel, offsetSpin);
    formLayout->addRow(phaseLabel, phaseSpin);
    formLayout->addRow(rampSymmetryLabel, rampSymmetrySpin);
    formLayout->addRow(pulseWidthLabel, pulseWidthSpin);
    formLayout->addRow(pulseRiseLabel, pulseRiseSpin);
    formLayout->addRow(pulseFallLabel, pulseFallSpin);
    formLayout->addRow(pulseDutyLabel, pulseDutySpin);
    formLayout->addRow(outputCheck);

    outerLayout->addWidget(groupBox);

    connect(waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                updateControlVisibility();
                emit waveformChanged(this->channel, waveform);
            });

    connect(frequencySpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                updatePulseDuty();
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

    connect(rampSymmetrySpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit rampSymmetryChanged(this->channel, value);
            });

    connect(pulseWidthSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                updatePulseDuty();

                emit pulseWidthChanged(
                    this->channel,
                    value);
            });

    connect(pulseRiseSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit pulseRiseChanged(
                    this->channel,
                    value / 1'000'000'000.0);
            });

    connect(pulseFallSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit pulseFallChanged(
                    this->channel,
                    value / 1'000'000'000.0);
            });

    connect(outputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit outputChanged(this->channel, enabled);
            });
}

void ChannelWidget::setFrequency(double frequency)
{
    frequencySpin->blockSignals(true);
    frequencySpin->setValue(frequency);
    frequencySpin->blockSignals(false);
    updatePulseDuty();
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

void ChannelWidget::setPhase(double value)
{
    phaseSpin->blockSignals(true);
    phaseSpin->setValue(value);
    phaseSpin->blockSignals(false);
}

void ChannelWidget::setRampSymmetry(double percent)
{
    rampSymmetrySpin->blockSignals(true);
    rampSymmetrySpin->setValue(percent);
    rampSymmetrySpin->blockSignals(false);
}

void ChannelWidget::setPulseWidth(double value)
{
    pulseWidthSpin->blockSignals(true);
    pulseWidthSpin->setValue(value);
    pulseWidthSpin->blockSignals(false);
    updatePulseDuty();
}

void ChannelWidget::setPulseRise(double value)
{
    pulseRiseSpin->blockSignals(true);
    pulseRiseSpin->setValue(value * 1'000'000'000.0);
    pulseRiseSpin->blockSignals(false);
}

void ChannelWidget::setPulseFall(double value)
{
    pulseFallSpin->blockSignals(true);
    pulseFallSpin->setValue(value * 1'000'000'000.0);
    pulseFallSpin->blockSignals(false);
}

void ChannelWidget::updatePulseDuty()
{
    if (waveformCombo->currentText() != "PULSE")
        return;

    const double frequency = frequencySpin->value();

    if (frequency <= 0.0)
    {
        pulseDutySpin->setValue(0.0);
        return;
    }

    const double duty =
        frequency * pulseWidthSpin->value() * 100.0;

    pulseDutySpin->setValue(duty);
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

void ChannelWidget::updateControlVisibility()
{
    const QString waveform = waveformCombo->currentText();

    const bool showSymmetry = (waveform == "RAMP");
    const bool showPulse = (waveform == "PULSE");

    rampSymmetryLabel->setVisible(showSymmetry);
    rampSymmetrySpin->setVisible(showSymmetry);

    pulseWidthLabel->setVisible(showPulse);
    pulseWidthSpin->setVisible(showPulse);

    pulseRiseLabel->setVisible(showPulse);
    pulseRiseSpin->setVisible(showPulse);

    pulseFallLabel->setVisible(showPulse);
    pulseFallSpin->setVisible(showPulse);

    pulseDutyLabel->setVisible(showPulse);
    pulseDutySpin->setVisible(showPulse);
}
