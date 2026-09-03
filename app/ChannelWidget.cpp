
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QResizeEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QApplication>
#include <QTimer>
#include <QtGlobal>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "ChannelWidget.h"
#include "StepAdjustSpinBox.h"
#include "QuantityEdit.h"
#include "AppController.h"
#include "ChannelState.h"
#include "debug.h"


namespace       // anonymous namespace so all within are at file scope
{

// Start of Amplitude section; still awaiting Vhigh/Vlow support
class AmplitudeRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"Vpp",   "Vpp",    1.0},
            {"mVpp",  "Vpp",    0.001},
            {"Vrms",  "Vrms",   1.0},
            {"mVrms", "Vrms",   0.001},
            {"dBm",   "dBm",    1.0}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "Vpp" };
    }

    double convert(double value,
                   const QString &from,
                   const QString &to) const override
    {
        if (from == to)
            return value;

        if (from == "Vpp" && to == "mVpp")
            return value * 1000.0;

        if (from == "mVpp" && to == "Vpp")
            return value / 1000.0;

        if (from == "Vrms" && to == "mVrms")
            return value * 1000.0;

        if (from == "mVrms" && to == "Vrms")
            return value / 1000.0;

        Q_ASSERT_X(false, "AmplitudeRepresentation::convert",
                   "unsupported amplitude conversion");
        return value;
    }
};

const AmplitudeRepresentation amplitudeQuantityRepresentation;

// Start of Offset section
class OffsetRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"Vdc",  "Vdc",  1.0},
            {"mVdc",  "Vdc",  0.001}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "Vdc" };
    }
};

const OffsetRepresentation offsetRepresentation;

// Start of Phase section

class PhaseRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return { {"°", "°", 1.0} };
    }

    QString canonicalRepresentation() const override
    {
        return {"°"};
    }
};

const PhaseRepresentation phaseRepresentation;

// Start of Frequency/Period section

class FrequencyRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"Hz",  "Hz",  1.0},
            {"kHz", "Hz", 1'000.0},
            {"MHz", "Hz", 1'000'000.0},
            {"mHz", "Hz", 0.001},
            {"uHz", "Hz", 0.000'001}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "Hz" };
    }
};

class PeriodRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"s",  "s",  1.0},
            {"ms", "s",  0.001},
            {"us", "s",  0.000'001},
            {"ns", "s",  0.000'000'001}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "s" };
    }
};

const FrequencyRepresentation frequencyQuantityRepresentation;
const PeriodRepresentation periodQuantityRepresentation;

}       // <<< end of anonymous namespace


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


ChannelWidget::ChannelWidget(AppController *controller, int my_channel,
                             QWidget *parent)
    : QWidget(parent),
      m_controller(controller),
      channel(my_channel)
{
    auto *outerLayout = new QVBoxLayout(this);

    auto *headerLayout = new QHBoxLayout;

    const QString chOutStr(QString("CH%1 Output").arg(channel));

    auto *titleLabel = new QLabel(chOutStr, this);

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

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);

    scrollContents = new QWidget;
    scrollArea->setWidget(scrollContents);

    outerLayout->addWidget(scrollArea);

    auto *scrollLayout = new QVBoxLayout(scrollContents);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSizeConstraint(QLayout::SetMinimumSize);

    groupBox = new QGroupBox(scrollContents);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    formLayout = new QFormLayout(groupBox);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    scrollLayout->addWidget(groupBox);

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

    outputCheck = new QCheckBox(chOutStr, groupBox);
    outputCheck->setObjectName("outputCheck");

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

    frequencyEdit = new QuantityEdit(m_controller,
                                frequencyQuantityRepresentation, groupBox);

    frequencyEdit->setObjectName("frequencyEdit");
    frequencyEdit->setToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");
    frequencyEdit->setMinimumWidth(215);
    frequencyEdit->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Fixed);
    frequencyEdit->setRange(0.000'01, 120'000'000);
    frequencyEdit->setDecimals(6);
    frequencyEdit->setSingleStep(0.000'01);
    frequencyEdit->setStepLimits(0.000'01, 100'000'000.0);
    frequencyEdit->setValue(1'000.0, "Hz");

    periodEdit = new QuantityEdit(m_controller, periodQuantityRepresentation,
                                  groupBox);
    periodEdit->setObjectName("periodEdit");
    periodEdit->setToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");

    periodEdit->setMinimumWidth(215);
    periodEdit->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    periodEdit->setRange(0.000'000'008'3, 1'000'000.0);
    periodEdit->setDecimals(6);
    periodEdit->setSingleStep(0.000'000'001);
    periodEdit->setStepLimits(0.000'000'000'001, 1'000'000.0);
    periodEdit->setValue(0.001, "s");

    amplitudeEdit = new QuantityEdit(m_controller,
                                     amplitudeQuantityRepresentation,
                                     groupBox);
    amplitudeEdit->setObjectName("amplitudeEdit");
    amplitudeEdit->setMinimumWidth(215);
    amplitudeEdit->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Fixed);

    offsetEdit = new QuantityEdit(m_controller, offsetRepresentation,
                                  groupBox);
    offsetEdit->setObjectName("offsetEdit");
    offsetEdit->setMinimumWidth(215);
    offsetEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    phaseSpin = new QuantityEdit(m_controller, phaseRepresentation,
                                 groupBox);
    phaseSpin->setObjectName("phaseSpin");
    phaseSpin->setRange(-360.0, 360.0);
    phaseSpin->setDecimals(1);
    phaseSpin->setSingleStep(1.0);
    phaseSpin->setStepLimits(0.1, 100.0);
    phaseSpin->setToolTip("Phase angle in degrees, from -360 to 360");

    dutySpin = new QDoubleSpinBox(groupBox);
    dutySpin->setObjectName("dutySpin");
    dutySpin->setRange(0.0, 100.0);
    dutySpin->setDecimals(1);
    dutySpin->setSingleStep(1.0);
    dutySpin->setSuffix("%");
    dutySpin->setKeyboardTracking(false);

    rampSymmetrySpin = new QDoubleSpinBox(groupBox);
    rampSymmetrySpin->setObjectName("rampSymmetrySpin");
    rampSymmetrySpin->setRange(0.0, 100.0);
    rampSymmetrySpin->setDecimals(1);
    rampSymmetrySpin->setSingleStep(1.0);
    rampSymmetrySpin->setSuffix(" %");
    rampSymmetrySpin->setKeyboardTracking(false);

    pulseWidthSpin = new QDoubleSpinBox(groupBox);
    pulseWidthSpin->setObjectName("pulseWidthSpin");
    pulseWidthSpin->setRange(0.000'000'001, 1.0);
    pulseWidthSpin->setDecimals(9);
    pulseWidthSpin->setSingleStep(0.000'001);
    pulseWidthSpin->setSuffix(" s");
    pulseWidthSpin->setKeyboardTracking(false);

    pulseRiseSpin = new QDoubleSpinBox(groupBox);
    pulseRiseSpin->setObjectName("pulseRiseSpin");
    pulseRiseSpin->setRange(0.001, 1'000'000.0);
    pulseRiseSpin->setDecimals(3);
    pulseRiseSpin->setSingleStep(0.1);
    pulseRiseSpin->setSuffix(" ns");
    pulseRiseSpin->setKeyboardTracking(false);

    pulseFallSpin = new QDoubleSpinBox(groupBox);
    pulseFallSpin->setObjectName("pulseFallSpin");
    pulseFallSpin->setRange(0.001, 1'000'000.0);
    pulseFallSpin->setDecimals(3);
    pulseFallSpin->setSingleStep(0.1);
    pulseFallSpin->setSuffix(" ns");
    pulseFallSpin->setKeyboardTracking(false);

    pulseDutySpin = new QDoubleSpinBox(groupBox);
    pulseDutySpin->setObjectName("pulseDutySpin");
    pulseDutySpin->setRange(0.0, 100.0);
    pulseDutySpin->setDecimals(3);
    pulseDutySpin->setSuffix(" %");
    pulseDutySpin->setReadOnly(true);
    pulseDutySpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    pulseDutySpin->setFocusPolicy(Qt::NoFocus);

    noiseStdevSpin = new QDoubleSpinBox(groupBox);
    noiseStdevSpin->setObjectName("noiseStdevSpin");
    noiseStdevSpin->setRange(0.002, 10.0);
    noiseStdevSpin->setDecimals(3);
    noiseStdevSpin->setSingleStep(0.001);
    noiseStdevSpin->setSuffix(" V");
    noiseStdevSpin->setKeyboardTracking(false);

    noiseMeanSpin = new QDoubleSpinBox(groupBox);
    noiseMeanSpin->setObjectName("noiseMeanSpin");
    noiseMeanSpin->setRange(-10.0, 10.0);
    noiseMeanSpin->setDecimals(3);
    noiseMeanSpin->setSingleStep(0.001);
    noiseMeanSpin->setSuffix(" V");
    noiseMeanSpin->setKeyboardTracking(false);

    noiseBandwidthSpin = new QDoubleSpinBox(groupBox);
    noiseBandwidthSpin->setObjectName("noiseBandwidthSpin");
    noiseBandwidthSpin->setRange(0.001, 120'000'000.0);
    noiseBandwidthSpin->setDecimals(3);
    noiseBandwidthSpin->setSingleStep(1.0);
    noiseBandwidthSpin->setSuffix(" Hz");
    noiseBandwidthSpin->setKeyboardTracking(false);

    noiseBandsetCheck = new QCheckBox(groupBox);
    noiseBandsetCheck->setObjectName("noiseBandsetCheck");
    noiseBandsetCheck->setText("On");

    dcOffsetSpin = new QDoubleSpinBox(groupBox);
    dcOffsetSpin->setObjectName("dcOffsetSpin");
    dcOffsetSpin->setRange(-10.000'0, 10.000'0);
    dcOffsetSpin->setDecimals(4);
    dcOffsetSpin->setSingleStep(1.0);
    dcOffsetSpin->setSuffix(" V");
    dcOffsetSpin->setKeyboardTracking(false);

    dcPrecisionHighCheck = new QCheckBox(groupBox);
    dcPrecisionHighCheck->setObjectName("dcPrecisionHighCheck");
    dcPrecisionHighCheck->setText("High");

    // Create widgets and labels
    waveformLabel = new QLabel("Waveform:", groupBox);
    frequencyLabel = new QLabel("Frequency:", groupBox);
    periodLabel = new QLabel("Period:", groupBox);
    amplitudeLabel = new QLabel("Amplitude:", groupBox);
    offsetLabel = new QLabel("Offset:", groupBox);
    phaseLabel = new QLabel("Phase:", groupBox);
    dutyLabel = new QLabel("Duty:", groupBox);
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

    // Add labels and related fields to form (which is in a groupbox)
    formLayout->addRow(waveformLabel, waveformCombo);
    formLayout->addRow(frequencyLabel, frequencyEdit);
    formLayout->addRow(periodLabel, periodEdit);
    formLayout->addRow(amplitudeLabel, amplitudeEdit);
    formLayout->addRow(offsetLabel, offsetEdit);
    formLayout->addRow(phaseLabel, phaseSpin);
    formLayout->addRow(dutyLabel, dutySpin);
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

    // updateControlVisibility() call is _after_ the connect() calls

    connect(waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                updateControlVisibility();
                emit waveformChanged(this->channel, waveform);
            });

    connect(frequencyEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(final);

                const double frequency = frequencyEdit->canonicalValue();

                if (frequency > 0.0) {
                    const double period = 1.0 / frequency;

                    periodEdit->setValue(period, "s");

                    sdgDebug() << Q_FUNC_INFO
                               << "frequency=" << frequency << "Hz"
                               << "period=" << period << "s";
                }
                else {
                    sdgDebug() << Q_FUNC_INFO
                               << "<< WILD frequency="
                               << frequency << "Hz >>";
                }

                updatePulseDuty();
                emit frequencyChanged(this->channel, frequency);
            });

    connect(amplitudeEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                sdgDebug() << objectName() << "Field contents:"
                           << amplitudeEdit->cleanText();
                sdgDebug() << offsetEdit->debugString();
                sdgDebug()
                    << "amplitude committed:"
                    << "original =" << original.value
                    << original.representation
                    << "final =" << final.value
                    << final.representation;

                emit amplitudeChanged(channel,
                                     final.value,
                                     final.representation);
#if 0   // too soon for this
                emit amplitudeChanged(channel,
                                      amplitudeEdit->canonicalValue(),
                                      final.representation);
                // Bridge to the existing amplitude/model code here.
#endif
            });

    connect(offsetEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &,
                   const QuantityEdit::Value &final)
            {
                sdgDebug() << objectName() << offsetEdit->debugString();
                sdgDebug()
                    << objectName()
                    << "offset committed:"
                    << "value =" << final.value
                    << "representation =" << final.representation;

                emit offsetChanged(channel, final.value,
                                   final.representation);
            });

    connect(phaseSpin,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << objectName() << phaseSpin->debugString();

                emit phaseChanged(this->channel, final.value);
            });

    connect(dutySpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double value)
            {
                emit dutyChanged(this->channel, value);
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

void ChannelWidget::setWaveformState(const QString &waveform)
{
    waveformCombo->blockSignals(true);
    waveformCombo->setCurrentText(waveform);
    waveformCombo->blockSignals(false);
    updateControlVisibility();
}

void ChannelWidget::setFrequencyState(double frequency)
{
    frequencyEdit->setValue(frequency, "Hz");

    if (frequency > 0.0) {
        const double period = 1.0 / frequency;

        periodEdit->setValue(period, "s");
        sdgDebug() << Q_FUNC_INFO
                   << "frequency=" << frequency << "Hz"
                   << "period=" << period << "s";
    }
    else
        sdgDebug() << Q_FUNC_INFO << "<< WILD frequency=" << frequency
                   << "Hz >>";

    updatePulseDuty();
}

// Going from internal state (where voltages are normalized) to UI
void ChannelWidget::setAmplitudeState(const AmplitudeState &amplit)
{
    const QString & rep { amplit.userRepresentation };

    if (rep == "Vpp" || rep == "mVpp")
    {
        double volts = amplit.getVpp();
        if (is_mV(rep))
            volts *= 1000.0;
        amplitudeEdit->setValue(volts, rep);
    }
    else if (rep == "Vrms" || rep == "mVrms")
    {
        double volts = amplit.getVrms();
        if (is_mV(rep))
            volts *= 1000.0;
        amplitudeEdit->setValue(volts, rep);
    }
    else if (rep == "dBm")
        amplitudeEdit->setValue(amplit.get_dBm(), rep);
    else if (rep.isEmpty())   // this case: Initial refresh after connect
    {
        sdgDebug() << objectName() << Q_FUNC_INFO << "defaulting to Vpp";
        amplitudeEdit->setValue(amplit.getVpp(), "Vpp");
    }
    else
        sdgDebug() << objectName() << Q_FUNC_INFO
                   << ">>> BAD representation: " << rep;
}

void ChannelWidget::setOffsetState(double offset)
{
    offsetEdit->setValue(offset, "Vdc");
}

void ChannelWidget::setPhaseState(double value)
{
    phaseSpin->setValue(value, "°");
}

void ChannelWidget::setDutyState(double value)
{
    dutySpin->blockSignals(true);
    dutySpin->setValue(value);
    dutySpin->blockSignals(false);
}

void ChannelWidget::setRampSymmetryState(double percent)
{
    rampSymmetrySpin->blockSignals(true);
    rampSymmetrySpin->setValue(percent);
    rampSymmetrySpin->blockSignals(false);
}

void ChannelWidget::setPulseWidthState(double value)
{
    pulseWidthSpin->blockSignals(true);
    pulseWidthSpin->setValue(value);
    pulseWidthSpin->blockSignals(false);
    updatePulseDuty();
}

void ChannelWidget::setPulseRiseState(double value)
{
    pulseRiseSpin->blockSignals(true);
    pulseRiseSpin->setValue(value * 1'000'000'000.0);
    pulseRiseSpin->blockSignals(false);
}

void ChannelWidget::setPulseFallState(double value)
{
    pulseFallSpin->blockSignals(true);
    pulseFallSpin->setValue(value * 1'000'000'000.0);
    pulseFallSpin->blockSignals(false);
}

void ChannelWidget::updatePulseDuty()
{
    if (waveformCombo->currentText() != "PULSE")
        return;

    const double frequency = frequencyEdit->canonicalValue();

    if (frequency <= 0.0)
    {
        pulseDutySpin->setValue(0.0);
        return;
    }

    const double duty =
        frequency * pulseWidthSpin->value() * 100.0;

    pulseDutySpin->setValue(duty);
}

void ChannelWidget::setNoiseBandsetState(bool enabled)
{
    noiseBandsetCheck->blockSignals(true);
    noiseBandsetCheck->setChecked(enabled);
    noiseBandsetCheck->blockSignals(false);

    updateControlVisibility();
}

void ChannelWidget::setNoiseStdevState(double value)
{
    noiseStdevSpin->blockSignals(true);
    noiseStdevSpin->setValue(value);
    noiseStdevSpin->blockSignals(false);
}

void ChannelWidget::setNoiseMeanState(double value)
{
    noiseMeanSpin->blockSignals(true);
    noiseMeanSpin->setValue(value);
    noiseMeanSpin->blockSignals(false);
}

void ChannelWidget::setNoiseBandwidthState(double value)
{
    noiseBandwidthSpin->blockSignals(true);
    noiseBandwidthSpin->setValue(value);
    noiseBandwidthSpin->blockSignals(false);
}

void ChannelWidget::setDcOffsetState(double value)
{
    dcOffsetSpin->blockSignals(true);
    dcOffsetSpin->setValue(value);
    dcOffsetSpin->blockSignals(false);
}

void ChannelWidget::setDcPrecisionHighState(bool enabled)
{
    dcPrecisionHighCheck->blockSignals(true);
    dcPrecisionHighCheck->setChecked(enabled);
    dcPrecisionHighCheck->blockSignals(false);
}

void ChannelWidget::setOutputState(const OutputState &output)
{
    outputCheck->blockSignals(true);
    outputCheck->setChecked(output.enabled);
    outputCheck->blockSignals(false);
}

void ChannelWidget::setStatus(const QString &text)
{
#ifdef SDG_DEVELOPER_UI
    statusLabel->setText(text);

 sdgDebug()
        << "CH" << channel
        << "status size" << statusLabel->size()
        << "status hint" << statusLabel->sizeHint()
        << "status minHint" << statusLabel->minimumSizeHint()
        << "group size" << groupBox->size()
        << "group hint" << groupBox->sizeHint()
        << "channel size" << size()
        << "channel hint" << sizeHint()
        << "amplitude size" << amplitudeEdit->size()
        << "amplitude hint" << amplitudeEdit->sizeHint();
#else
    Q_UNUSED(text);
#endif
}

void ChannelWidget::updateControlVisibility()
{
    const QString waveform = waveformCombo->currentText();

    const bool showSquare = (waveform == "SQUARE");
    const bool showSymmetry = (waveform == "RAMP");
    const bool showPulse = (waveform == "PULSE");
    const bool showNoise = (waveform == "NOISE");
    const bool showDC = (waveform == "DC");
    const bool showStandardControls = !showNoise && !showDC;

    frequencyLabel->setVisible(showStandardControls);
    frequencyEdit->setVisible(showStandardControls);

    amplitudeLabel->setVisible(showStandardControls);
    amplitudeEdit->setVisible(showStandardControls);

    offsetLabel->setVisible(showStandardControls);
    offsetEdit->setVisible(showStandardControls);

    phaseLabel->setVisible(showStandardControls);
    phaseSpin->setVisible(showStandardControls);

    dutyLabel->setVisible(showSquare);
    dutySpin->setVisible(showSquare);

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

void ChannelWidget::debugLayout() const
{
    sdgDebug()
        << "CH" << channel
        << "widget" << size()
        << "hint" << sizeHint()
        << "minHint" << minimumSizeHint()
        << "group" << groupBox->size()
        << "groupHint" << groupBox->sizeHint()
        << "ampEdit" << amplitudeEdit->size()
        << "ampEditHint" << amplitudeEdit->sizeHint()
        << "ampMinHint" << amplitudeEdit->minimumSizeHint();
}
