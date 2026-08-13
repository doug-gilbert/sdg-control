
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QResizeEvent>
#include <QPushButton>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "debug.h"
#include "ChannelWidget.h"


// Helper class ChannelGroupBox hidden in this source file
class ChannelGroupBox : public QGroupBox
{
public:
    using QGroupBox::QGroupBox;

    QToolButton *closeButton = nullptr;

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QGroupBox::resizeEvent(event);

        if (closeButton)
        {
            closeButton->adjustSize();

            closeButton->move(
                width() - closeButton->width() - 4,
                1);
        }
    }
};


ChannelWidget::ChannelWidget(int my_channel, QWidget *parent)
    : QWidget(parent),
      channel(my_channel)
{
    auto *outerLayout = new QVBoxLayout(this);

    auto *headerLayout = new QHBoxLayout;

    auto *titleLabel = new QLabel(
        QString("Channel %1").arg(channel),
        this);

    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);

    closeButton = new QPushButton("x", this);
    closeButton->setFixedSize(28, 28);
    closeButton->setToolTip("Hide channel");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton);

    outerLayout->addLayout(headerLayout);

    groupBox = new QGroupBox(this);
    formLayout = new QFormLayout(groupBox);

    outerLayout->addWidget(groupBox);

#ifdef SDG_DEVELOPER_UI
    statusLabel = new QLabel(
        QString("CH%1: --").arg(channel),
        groupBox);

    statusLabel->setWordWrap(true);
    statusLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred);
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
        "DC",  // Supported by the SDG, but DC-specific UI not implemented yet
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

    noiseStdevSpin = new QDoubleSpinBox(groupBox);
    noiseStdevSpin->setRange(0.002, 10.0);
    noiseStdevSpin->setDecimals(3);
    noiseStdevSpin->setSingleStep(0.001);
    noiseStdevSpin->setSuffix(" V");
    noiseStdevSpin->setKeyboardTracking(false);

    noiseMeanSpin = new QDoubleSpinBox(groupBox);
    noiseMeanSpin->setRange(-10.0, 10.0);
    noiseMeanSpin->setDecimals(3);
    noiseMeanSpin->setSingleStep(0.001);
    noiseMeanSpin->setSuffix(" V");
    noiseMeanSpin->setKeyboardTracking(false);

    noiseBandwidthSpin = new QDoubleSpinBox(groupBox);
    noiseBandwidthSpin->setRange(0.001, 120'000'000.0);
    noiseBandwidthSpin->setDecimals(3);
    noiseBandwidthSpin->setSingleStep(1.0);
    noiseBandwidthSpin->setSuffix(" Hz");
    noiseBandwidthSpin->setKeyboardTracking(false);

    noiseBandsetCheck = new QCheckBox(groupBox);
    noiseBandsetCheck->setText("On");

    dcOffsetSpin = new QDoubleSpinBox(groupBox);
    dcOffsetSpin->setRange(-10.000'0, 10.000'0);
    dcOffsetSpin->setDecimals(4);
    dcOffsetSpin->setSingleStep(1.0);
    dcOffsetSpin->setSuffix(" V");
    dcOffsetSpin->setKeyboardTracking(false);

    dcPrecisionHighCheck = new QCheckBox(groupBox);
    dcPrecisionHighCheck->setText("High");

    // Create widgets and labels
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
    noiseStdevLabel = new QLabel("Noise Stdev:", groupBox);
    noiseMeanLabel = new QLabel("Noise Mean:", groupBox);
    noiseBandwidthLabel = new QLabel("Bandwidth:", groupBox);
    noiseBandsetLabel = new QLabel("Bandset:", groupBox);
    dcOffsetLabel = new QLabel("DC Offset:", groupBox);
    dcPrecisionHighLabel = new QLabel("DC Precision:", groupBox);

    updateControlVisibility();

    // Add labels and related fields to form (which is in groupbox)
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
    formLayout->addRow(noiseBandsetLabel, noiseBandsetCheck);
    formLayout->addRow(noiseStdevLabel, noiseStdevSpin);
    formLayout->addRow(noiseMeanLabel, noiseMeanSpin);
    formLayout->addRow(noiseBandwidthLabel, noiseBandwidthSpin);
    formLayout->addRow(dcOffsetLabel, dcOffsetSpin);
    formLayout->addRow(dcPrecisionHighLabel, dcPrecisionHighCheck);

    formLayout->addRow(outputCheck);

    // Important step
    outerLayout->addWidget(groupBox);

    // updateControlVisibility() call is _after_ the connect() calls

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

    connect(noiseBandsetCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                updateControlVisibility();
                emit noiseBandsetChanged(channel, enabled);
            });

    connect(noiseStdevSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                emit noiseStdevChanged(channel, value);
            });

    connect(noiseMeanSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                emit noiseMeanChanged(channel, value);
            });

    connect(noiseBandwidthSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                emit noiseBandwidthChanged(channel, value);
            });

    connect(dcOffsetSpin,
            &QDoubleSpinBox::valueChanged,
            this,
            [this](double value)
            {
                emit dcOffsetChanged(channel, value);
            });

    connect(dcPrecisionHighCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit dcPrecisionHighChanged(channel, enabled);
            });


    connect(outputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit outputChanged(this->channel, enabled);
            });

    connect(closeButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                sdgDebug() << "Close clicked for channel" << channel;
                emit hideRequested(channel);
            });

    // This sets initial visibilty (whether or not fields are shown)
    updateControlVisibility();
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

void ChannelWidget::setNoiseBandset(bool enabled)
{
    noiseBandsetCheck->blockSignals(true);
    noiseBandsetCheck->setChecked(enabled);
    noiseBandsetCheck->blockSignals(false);

    updateControlVisibility();
}

void ChannelWidget::setNoiseStdev(double value)
{
    noiseStdevSpin->blockSignals(true);
    noiseStdevSpin->setValue(value);
    noiseStdevSpin->blockSignals(false);
}

void ChannelWidget::setNoiseMean(double value)
{
    noiseMeanSpin->blockSignals(true);
    noiseMeanSpin->setValue(value);
    noiseMeanSpin->blockSignals(false);
}

void ChannelWidget::setNoiseBandwidth(double value)
{
    noiseBandwidthSpin->blockSignals(true);
    noiseBandwidthSpin->setValue(value);
    noiseBandwidthSpin->blockSignals(false);
}

void ChannelWidget::setDcOffset(double value)
{
    dcOffsetSpin->blockSignals(true);
    dcOffsetSpin->setValue(value);
    dcOffsetSpin->blockSignals(false);
}

void ChannelWidget::setDcPrecisionHigh(bool enabled)
{
    dcPrecisionHighCheck->blockSignals(true);
    dcPrecisionHighCheck->setChecked(enabled);
    dcPrecisionHighCheck->blockSignals(false);
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
    const bool showNoise = (waveform == "NOISE");
    const bool showDC = (waveform == "DC");
    const bool showStandardControls = !showNoise && !showDC;

    frequencyLabel->setVisible(showStandardControls);
    frequencySpin->setVisible(showStandardControls);

    amplitudeLabel->setVisible(showStandardControls);
    amplitudeSpin->setVisible(showStandardControls);

    offsetLabel->setVisible(showStandardControls);
    offsetSpin->setVisible(showStandardControls);

    phaseLabel->setVisible(showStandardControls);
    phaseSpin->setVisible(showStandardControls);

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

    const bool showNoiseBandwidth =
        showNoise && noiseBandsetCheck->isChecked();

    noiseBandsetLabel->setVisible(showNoise);
    noiseBandsetCheck->setVisible(showNoise);

    noiseStdevLabel->setVisible(showNoise);
    noiseStdevSpin->setVisible(showNoise);

    noiseMeanLabel->setVisible(showNoise);
    noiseMeanSpin->setVisible(showNoise);

    noiseBandwidthLabel->setVisible(showNoiseBandwidth);
    noiseBandwidthSpin->setVisible(showNoiseBandwidth);

    dcOffsetLabel->setVisible(showDC);
    dcOffsetSpin->setVisible(showDC);

    dcPrecisionHighLabel->setVisible(showDC);
    dcPrecisionHighCheck->setVisible(showDC);
}

void ChannelWidget::setControlsEnabled(bool enabled)
{
    groupBox->setEnabled(enabled);
}
