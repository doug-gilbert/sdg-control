/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

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
#include <QMenu>

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

// Start of Duty section

class DutyRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return { {"%", "%", 1.0} };
    }

    QString canonicalRepresentation() const override
    {
        return {"%"};
    }
};

const DutyRepresentation dutyRepresentation;

// Start of RampSymmetry section

class RampSymmetryRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return { {"%", "%", 1.0} };
    }

    QString canonicalRepresentation() const override
    {
        return {"%"};
    }
};

const RampSymmetryRepresentation rampSymmetryRepresentation;

// Start of PulseWidth section

class PulseWidthRepresentation : public QuantityRepresentation
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
        return {"s"};
    }
};

const PulseWidthRepresentation pulseWidthRepresentation;

// Start of PulseRise section

class PulseRiseRepresentation : public QuantityRepresentation
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
        return {"s"};
    }
};

const PulseRiseRepresentation pulseRiseRepresentation;

// Start of PulseFall section

class PulseFallRepresentation : public QuantityRepresentation
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
        return {"s"};
    }
};

const PulseFallRepresentation pulseFallRepresentation;

// Start of PulseDuty section

class PulseDutyRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return { {"%", "%", 1.0} };
    }

    QString canonicalRepresentation() const override
    {
        return {"%"};
    }
};

const PulseDutyRepresentation pulseDutyRepresentation;

// Start of NoiseStdev section
class NoiseStdevRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"V",  "V",  1.0},
            {"mV",  "V",  0.001}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "V" };
    }
};

const NoiseStdevRepresentation noiseStdevRepresentation;

// Start of NoiseMean section
class NoiseMeanRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"V",  "V",  1.0},
            {"mV",  "V",  0.001}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "V" };
    }
};

const NoiseMeanRepresentation noiseMeanRepresentation;

// Start of NoiseBandwidth section
class NoiseBandwidthRepresentation : public QuantityRepresentation
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

const NoiseBandwidthRepresentation noiseBandwidthRepresentation;

// Start of DcOffset section
class DcOffsetRepresentation : public QuantityRepresentation
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

const DcOffsetRepresentation dcOffsetRepresentation;

}       // <<< end of anonymous namespace


// Helper class ChannelGroupBox hidden in this source file
class ChannelGroupBox : public QGroupBox
{
public:
    using QGroupBox::QGroupBox;

    QToolButton *m_closeButton = nullptr;

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QGroupBox::resizeEvent(event);

        if (m_closeButton)
        {
            m_closeButton->adjustSize();

            m_closeButton->move(
                width() - m_closeButton->width() - 4,
                1);
        }
    }
};


ChannelWidget::ChannelWidget(AppController *controller, int my_channel,
                             const ChannelDirtyState *dirtyState,
                             QWidget *parent)
    : QWidget(parent),
      m_controller(controller),
      m_channel(my_channel),
      m_dirtyState(dirtyState)
{
    auto *outerLayout = new QVBoxLayout(this);

    auto *headerLayout = new QHBoxLayout;

    const QString chOutStr(QString("CH%1 Output").arg(m_channel));

    auto *titleLabel = new QLabel(chOutStr, this);

    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);

    m_closeButton = new QPushButton("x", this);
    m_closeButton->setFixedSize(28, 28);
    m_closeButton->setToolTip("Hide channel");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_closeButton);

    outerLayout->addLayout(headerLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);

    m_scrollContents = new QWidget;
    m_scrollArea->setWidget(m_scrollContents);

    outerLayout->addWidget(m_scrollArea);

    auto *scrollLayout = new QVBoxLayout(m_scrollContents);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSizeConstraint(QLayout::SetMinimumSize);

    m_groupBox = new QGroupBox(m_scrollContents);
    m_groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_formLayout = new QFormLayout(m_groupBox);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    scrollLayout->addWidget(m_groupBox);

#ifdef SDG_DEVELOPER_UI
    m_statusLabel = new QLabel(
        QString("CH%1: --").arg(m_channel),
        m_groupBox);

    m_statusLabel->setWordWrap(true);
    m_statusLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred);
    m_statusLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard);

    m_formLayout->addRow("Status:", m_statusLabel);
#endif

    m_outputCheck = new QCheckBox(chOutStr, m_groupBox);
    m_outputCheck->setObjectName("outputCheck");

    m_waveformCombo = new QComboBox(m_groupBox);

    m_waveformCombo->addItems({
        "SINE",
        "SQUARE",
        "RAMP",
        "PULSE",
        "NOISE",
        "DC",  // Supported by the SDG, but DC-specific UI not implemented yet
        "ARB"
    });

    m_frequencyEdit = new QuantityEdit(m_controller,
                                       frequencyQuantityRepresentation,
                                       m_dirtyState->m_frequency,
                                       m_groupBox);
    m_frequencyEdit->setObjectName("frequencyEdit");
    m_frequencyEdit->setToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");
    m_frequencyEdit->setMinimumWidth(215);
    m_frequencyEdit->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Fixed);
    m_frequencyEdit->setCanonicalRange(0.000'01, 120'000'000);
    m_frequencyEdit->setDecimals(6);
    m_frequencyEdit->setSingleStep(0.000'01);
    m_frequencyEdit->setStepLimits(0.000'01, 100'000'000.0);
    m_frequencyEdit->setValue(1'000.0, "Hz");

    m_periodEdit = new QuantityEdit(m_controller,
                                    periodQuantityRepresentation,
          /* not an error --> */    m_dirtyState->m_frequency,
                                    m_groupBox);
    m_periodEdit->setObjectName("periodEdit");
    m_periodEdit->setToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");

    m_periodEdit->setMinimumWidth(215);
    m_periodEdit->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    m_periodEdit->setCanonicalRange(0.000'000'008'3, 1'000'000.0);
    m_periodEdit->setDecimals(6);
    m_periodEdit->setSingleStep(0.000'000'001);
    m_periodEdit->setStepLimits(0.000'000'000'001, 1'000'000.0);
    m_periodEdit->setValue(0.001, "s");

    m_amplitudeEdit = new QuantityEdit(m_controller,
                                       amplitudeQuantityRepresentation,
                                       m_dirtyState->m_amplitude,
                                       m_groupBox);
    m_amplitudeEdit->setObjectName("amplitudeEdit");
    m_amplitudeEdit->setMinimumWidth(215);
    m_amplitudeEdit->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Fixed);

    m_offsetEdit = new QuantityEdit(m_controller,
                                    offsetRepresentation,
                                    m_dirtyState->m_offset,
                                    m_groupBox);
    m_offsetEdit->setObjectName("offsetEdit");
    m_offsetEdit->setMinimumWidth(215);
    m_offsetEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_phaseEdit = new QuantityEdit(m_controller,
                                   phaseRepresentation,
                                   m_dirtyState->m_phase,
                                   m_groupBox);
    m_phaseEdit->setObjectName("phaseEdit");
    // No comboBox so we can call setrange() directly
    m_phaseEdit->setRange(-360.0, 360.0);
    m_phaseEdit->setDecimals(1);
    m_phaseEdit->setSingleStep(1.0);
    m_phaseEdit->setStepLimits(0.1, 100.0);
    m_phaseEdit->setToolTip("Phase angle in degrees, from -360 to 360");

    m_dutyEdit = new QuantityEdit(m_controller, dutyRepresentation,
                                   m_dirtyState->m_duty, m_groupBox);
    m_dutyEdit->setObjectName("dutyEdit");
    // No comboBox so we can call setrange() directly
    m_dutyEdit->setRange(0.0, 100.0);
    m_dutyEdit->setDecimals(1);
    m_dutyEdit->setSingleStep(1.0);
    m_dutyEdit->setStepLimits(0.1, 10.0);
    m_dutyEdit->setToolTip(
                  "Duty cycle: time_up/(time_up+time_down) as percentage");

    m_rampSymmetryEdit = new QuantityEdit(m_controller,
                                          rampSymmetryRepresentation,
                                          m_dirtyState->m_rampSymmetry,
                                          m_groupBox);
    m_rampSymmetryEdit->setObjectName("rampSymmetryEdit");
    // No comboBox so we can call setrange() directly
    m_rampSymmetryEdit->setRange(0.0, 100.0);
    m_rampSymmetryEdit->setDecimals(1);
    m_rampSymmetryEdit->setSingleStep(1.0);
    m_rampSymmetryEdit->setToolTip("(ramp_up / (ramp_up+ramp_down)) * 100");

    m_pulseWidthEdit = new QuantityEdit(m_controller,
                                        pulseWidthRepresentation,
                                        m_dirtyState->m_pulseWidth,
                                        m_groupBox);
    m_pulseWidthEdit->setObjectName("pulseWidthEdit");
    m_pulseWidthEdit->setCanonicalRange(0.000'000'001, 1.0);
    m_pulseWidthEdit->setDecimals(9);
    m_pulseWidthEdit->setSingleStep(0.000'001);

    m_pulseRiseEdit = new QuantityEdit(m_controller,
                                       pulseRiseRepresentation,
                                       m_dirtyState->m_pulseRise,
                                       m_groupBox);
    m_pulseRiseEdit->setObjectName("pulseRiseEdit");
    // m_pulseRiseEdit->setRange(0.001, 1'000'000.0);
    m_pulseRiseEdit->setDecimals(3);
    m_pulseRiseEdit->setSingleStep(0.1);

    m_pulseFallEdit = new QuantityEdit(m_controller,
                                       pulseFallRepresentation,
                                       m_dirtyState->m_pulseFall,
                                       m_groupBox);
    m_pulseFallEdit->setObjectName("pulseFallEdit");
    // m_pulseFallEdit->setRange(0.001, 1'000'000.0);
    m_pulseFallEdit->setDecimals(3);
    m_pulseFallEdit->setSingleStep(0.1);

    m_pulseDutyEdit = new QuantityEdit(m_controller, pulseDutyRepresentation,
          /* duty or pulseDuty ?? */   m_dirtyState->m_duty,
                                       m_groupBox);
    m_pulseDutyEdit->setObjectName("pulseDutyEdit");
    // No comboBox so we can call setrange() directly
    m_pulseDutyEdit->setRange(0.0, 100.0);
    m_pulseDutyEdit->setDecimals(1);
    m_pulseDutyEdit->setSingleStep(1.0);
    m_pulseDutyEdit->setStepLimits(0.1, 10.0);
    m_pulseDutyEdit->setToolTip(
                  "Duty cycle: time_up/(time_up+time_down) as percentage");

    m_noiseStdevEdit = new QuantityEdit(m_controller, noiseStdevRepresentation,
                                        m_dirtyState->m_noiseStdev,
                                        m_groupBox);
    m_noiseStdevEdit->setObjectName("noiseStdevEdit");
    m_noiseStdevEdit->setCanonicalRange(0.002, 10.0);
    m_noiseStdevEdit->setDecimals(3);
    m_noiseStdevEdit->setSingleStep(0.001);

    m_noiseMeanEdit = new QuantityEdit(m_controller, noiseMeanRepresentation,
                                       m_dirtyState->m_noiseMean,
                                       m_groupBox);
    m_noiseMeanEdit->setObjectName("noiseMeanEdit");
    m_noiseMeanEdit->setCanonicalRange(-10.0, 10.0);
    m_noiseMeanEdit->setDecimals(3);
    m_noiseMeanEdit->setSingleStep(0.001);

    m_noiseBandwidthEdit = new QuantityEdit(m_controller,
                                            noiseBandwidthRepresentation,
                                            m_dirtyState->m_noiseBandwidth,
                                            m_groupBox);
    m_noiseBandwidthEdit->setObjectName("noiseBandwidthEdit");
    m_noiseBandwidthEdit->setCanonicalRange(0.000'001, 120'000'000.0);
    m_noiseBandwidthEdit->setDecimals(3);
    m_noiseBandwidthEdit->setSingleStep(1.0);

    m_noiseBandsetCheck = new QCheckBox(m_groupBox);
    m_noiseBandsetCheck->setObjectName("noiseBandsetCheck");
    m_noiseBandsetCheck->setText("On");

    m_dcOffsetEdit = new QuantityEdit(m_controller, dcOffsetRepresentation,
                                      m_dirtyState->m_dcOffset,
                                      m_groupBox);
    m_dcOffsetEdit->setObjectName("dcOffsetEdit");
    m_dcOffsetEdit->setCanonicalRange(-10.000'0, 10.000'0);
    m_dcOffsetEdit->setDecimals(4);
    m_dcOffsetEdit->setSingleStep(1.0);

    m_dcPrecisionHighCheck = new QCheckBox(m_groupBox);
    m_dcPrecisionHighCheck->setObjectName("dcPrecisionHighCheck");
    m_dcPrecisionHighCheck->setText("High");

    // Create widgets and labels
    m_waveformLabel = new QLabel("Waveform:", m_groupBox);
    m_frequencyLabel = new QLabel("Frequency:", m_groupBox);
    m_periodLabel = new QLabel("Period:", m_groupBox);
    m_amplitudeLabel = new QLabel("Amplitude:", m_groupBox);
    m_offsetLabel = new QLabel("Offset:", m_groupBox);
    m_phaseLabel = new QLabel("Phase:", m_groupBox);
    m_dutyLabel = new QLabel("Duty:", m_groupBox);
    m_rampSymmetryLabel = new QLabel("Ramp symmetry:", m_groupBox);
    m_pulseWidthLabel = new QLabel("Pulse Width:", m_groupBox);
    m_pulseRiseLabel = new QLabel("Pulse Rise:", m_groupBox);
    m_pulseFallLabel = new QLabel("Pulse Fall:", m_groupBox);
    m_pulseDutyLabel = new QLabel("Pulse Duty:", m_groupBox);
    m_noiseStdevLabel = new QLabel("Noise Stdev:", m_groupBox);
    m_noiseMeanLabel = new QLabel("Noise Mean:", m_groupBox);
    m_noiseBandwidthLabel = new QLabel("Bandwidth:", m_groupBox);
    m_noiseBandsetLabel = new QLabel("Bandset:", m_groupBox);
    m_dcOffsetLabel = new QLabel("DC Offset:", m_groupBox);
    m_dcPrecisionHighLabel = new QLabel("DC Precision:", m_groupBox);

    updateControlVisibility();

    // Add labels and related fields to form (which is in a groupbox)
    m_formLayout->addRow(m_waveformLabel, m_waveformCombo);
    m_formLayout->addRow(m_frequencyLabel, m_frequencyEdit);
    m_formLayout->addRow(m_periodLabel, m_periodEdit);
    m_formLayout->addRow(m_amplitudeLabel, m_amplitudeEdit);
    m_formLayout->addRow(m_offsetLabel, m_offsetEdit);
    m_formLayout->addRow(m_phaseLabel, m_phaseEdit);
    m_formLayout->addRow(m_dutyLabel, m_dutyEdit);
    m_formLayout->addRow(m_rampSymmetryLabel, m_rampSymmetryEdit);
    m_formLayout->addRow(m_pulseWidthLabel, m_pulseWidthEdit);
    m_formLayout->addRow(m_pulseRiseLabel, m_pulseRiseEdit);
    m_formLayout->addRow(m_pulseFallLabel, m_pulseFallEdit);
    m_formLayout->addRow(m_pulseDutyLabel, m_pulseDutyEdit);
    m_formLayout->addRow(m_noiseBandsetLabel, m_noiseBandsetCheck);
    m_formLayout->addRow(m_noiseStdevLabel, m_noiseStdevEdit);
    m_formLayout->addRow(m_noiseMeanLabel, m_noiseMeanEdit);
    m_formLayout->addRow(m_noiseBandwidthLabel, m_noiseBandwidthEdit);
    m_formLayout->addRow(m_dcOffsetLabel, m_dcOffsetEdit);
    m_formLayout->addRow(m_dcPrecisionHighLabel, m_dcPrecisionHighCheck);

    m_formLayout->addRow(m_outputCheck);

    // updateControlVisibility() call is _after_ the connect() calls

    connect(m_waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                updateControlVisibility();
                emit waveformChanged(this->m_channel, waveform);
            });

    connect(m_frequencyEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(final);

                const double frequency = m_frequencyEdit->canonicalValue();

                if (frequency > 0.0) {
                    const double period = 1.0 / frequency;

                    m_periodEdit->setValue(period, "s");

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
                emit frequencyChanged(this->m_channel, frequency);
            });

    connect(m_periodEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(final);

                const double period = m_periodEdit->canonicalValue();

                if (period > 0.0) {
                    const double frequency = 1.0 / period;

                    m_frequencyEdit->setValue(frequency, "Hz");

                    sdgDebug() << Q_FUNC_INFO
                               << "period=" << period << "s"
                               << "frequency=" << frequency << "Hz";
                }
                else {
                    sdgDebug() << Q_FUNC_INFO
                               <<  "<< WILD period=" << period << "s >>";
                }

                updatePulseDuty();
                emit periodChanged(this->m_channel, period);
            });

    connect(m_amplitudeEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                sdgDebug() << "Amplitude field contents:"
                           << m_amplitudeEdit->cleanText();
                sdgDebug() << m_offsetEdit->debugString();
                sdgDebug()
                    << "amplitude committed:"
                    << "original =" << original.value
                    << original.representation
                    << "final =" << final.value
                    << final.representation;

                emit amplitudeChanged(m_channel,
                                     final.value,
                                     final.representation);
#if 0   // too soon for this
                emit amplitudeChanged(m_channel,
                                      m_amplitudeEdit->canonicalValue(),
                                      final.representation);
                // Bridge to the existing amplitude/model code here.
#endif
            });

    connect(m_offsetEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &,
                   const QuantityEdit::Value &final)
            {
                sdgDebug() << m_offsetEdit->debugString();
                sdgDebug()
                    << objectName()
                    << "offset committed:"
                    << "value =" << final.value
                    << "representation =" << final.representation;

                emit offsetChanged(m_channel, final.value,
                                   final.representation);
            });

    connect(m_phaseEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_phaseEdit->debugString();

                emit phaseChanged(this->m_channel, final.value);
            });

    connect(m_dutyEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_dutyEdit->debugString();

                emit dutyChanged(this->m_channel, final.value);
            });

    connect(m_rampSymmetryEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_rampSymmetryEdit->debugString();

                emit rampSymmetryChanged(this->m_channel, final.value);
            });

    connect(m_pulseWidthEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_pulseWidthEdit->debugString();

                emit pulseWidthChanged(this->m_channel, final.value);
            });

    connect(m_pulseRiseEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_pulseRiseEdit->debugString();
#if 1
                emit pulseRiseChanged(this->m_channel, final.value);
#else
                emit pulseRiseChanged(
                    this->m_channel,
                    value / 1'000'000'000.0);
#endif
            });

    connect(m_pulseFallEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_pulseFallEdit->debugString();
#if 1
                emit pulseFallChanged(this->m_channel, final.value);
#else
                emit pulseFallChanged(
                    this->m_channel,
                    value / 1'000'000'000.0);
#endif
            });

    connect(m_noiseBandsetCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                updateControlVisibility();
                emit noiseBandsetChanged(m_channel, enabled);
            });

    connect(m_noiseStdevEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_noiseStdevEdit->debugString();

                emit noiseStdevChanged(this->m_channel, final.value);
            });

    connect(m_noiseMeanEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_noiseMeanEdit->debugString();

                emit noiseMeanChanged(this->m_channel, final.value);
            });

    connect(m_noiseBandwidthEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_noiseBandwidthEdit->debugString();

                emit noiseBandwidthChanged(this->m_channel, final.value);
            });

    connect(m_dcOffsetEdit,
            &QuantityEdit::committed,
            this,
            [this](const QuantityEdit::Value &original,
                   const QuantityEdit::Value &final)
            {
                Q_UNUSED(original);
                sdgDebug() << m_dcOffsetEdit->debugString();

                emit dcOffsetChanged(this->m_channel, final.value);
            });

    connect(m_dcPrecisionHighCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit dcPrecisionHighChanged(m_channel, enabled);
            });


    connect(m_outputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit outputChanged(this->m_channel, enabled);
            });

    connect(m_closeButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                sdgDebug() << "Close clicked for channel" << m_channel;
                emit hideRequested(m_channel);
            });

    // This sets initial visibilty (whether or not fields are shown)
    updateControlVisibility();

}

void ChannelWidget::setUiWaveform(const QString &waveform)
{
    m_waveformCombo->blockSignals(true);
    m_waveformCombo->setCurrentText(waveform);
    m_waveformCombo->blockSignals(false);
    updateControlVisibility();
}

void ChannelWidget::setUiFrequency(double frequency)
{
    m_frequencyEdit->setCanonicalValue(frequency);

    if (frequency > 0.0) {
        const double period = 1.0 / frequency;

        m_periodEdit->setValue(period, "s");
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
void ChannelWidget::setUiAmplitude(const AmplitudeState &amplit)
{
    const QString & rep { amplit.userRepresentation };

    if (rep == "Vpp" || rep == "mVpp")
    {
        double volts = amplit.getVpp();
        if (is_mV(rep))
            volts *= 1000.0;
        m_amplitudeEdit->setValue(volts, rep);
    }
    else if (rep == "Vrms" || rep == "mVrms")
    {
        double volts = amplit.getVrms();
        if (is_mV(rep))
            volts *= 1000.0;
        m_amplitudeEdit->setValue(volts, rep);
    }
    else if (rep == "dBm")
        m_amplitudeEdit->setValue(amplit.get_dBm(), rep);
    else if (rep.isEmpty())   // this case: Initial refresh after connect
    {
        sdgDebug() << objectName() << Q_FUNC_INFO << "defaulting to Vpp";
        m_amplitudeEdit->setValue(amplit.getVpp(), "Vpp");
    }
    else
        sdgDebug() << objectName() << Q_FUNC_INFO
                   << ">>> BAD representation: " << rep;
}

void ChannelWidget::setUiOffset(double offset)
{
    m_offsetEdit->setValue(offset, "Vdc");
}

void ChannelWidget::setUiPhase(double value)
{
    m_phaseEdit->setValue(value, "°");
}

void ChannelWidget::setUiDuty(double value)
{
    m_dutyEdit->setValue(value, "%");
}

void ChannelWidget::setUiRampSymmetry(double percent)
{
    m_rampSymmetryEdit->setValue(percent, "%");
}

void ChannelWidget::setUiPulseWidth(double value)
{
    m_pulseWidthEdit->setValue(value, "s");
    updatePulseDuty();
}

void ChannelWidget::setUiPulseRise(double value)
{
    // m_pulseRiseEdit->setValue(value * 1'000'000'000.0);
    m_pulseRiseEdit->setValue(value, "s");
}

void ChannelWidget::setUiPulseFall(double value)
{
    // m_pulseFallEdit->setValue(value * 1'000'000'000.0);
    m_pulseFallEdit->setValue(value, "s");
}

void ChannelWidget::updatePulseDuty()
{
    if (m_waveformCombo->currentText() != "PULSE")
        return;

    const double frequency = m_frequencyEdit->canonicalValue();

    if (frequency <= 0.0)
    {
        m_pulseDutyEdit->setValue(0.0, "Hz");
        return;
    }

    const double duty =
        frequency * m_pulseWidthEdit->value().value * 100.0;

    // Do we need duty_orig to see if this was a change or not
    m_pulseDutyEdit->setValue(duty, "%");
}

void ChannelWidget::setUiNoiseBandset(bool enabled)
{
    m_noiseBandsetCheck->setChecked(enabled);

    updateControlVisibility();
}

void ChannelWidget::setUiNoiseStdev(double value)
{
    m_noiseStdevEdit->setValue(value, "V");
}

void ChannelWidget::setUiNoiseMean(double value)
{
    m_noiseMeanEdit->setValue(value, "V");
}

void ChannelWidget::setUiNoiseBandwidth(double value)
{
    m_noiseBandwidthEdit->setValue(value, "Hz");
}

void ChannelWidget::setUiDcOffset(double value)
{
    m_dcOffsetEdit->setValue(value, "Vdc");
}

void ChannelWidget::setUiDcPrecisionHigh(bool enabled)
{
    m_dcPrecisionHighCheck->blockSignals(true);
    m_dcPrecisionHighCheck->setChecked(enabled);
    m_dcPrecisionHighCheck->blockSignals(false);
}

void ChannelWidget::setUiOutput(const OutputState &output)
{
    m_outputCheck->blockSignals(true);
    m_outputCheck->setChecked(output.enabled);
    m_outputCheck->blockSignals(false);
}

void ChannelWidget::setUiStatus(const QString &text)
{
#ifdef SDG_DEVELOPER_UI
    m_statusLabel->setText(text);
#else
    Q_UNUSED(text);
#endif
}

void ChannelWidget::updateControlVisibility()
{
    const QString waveform = m_waveformCombo->currentText();

    const bool showSquare = (waveform == "SQUARE");
    const bool showSymmetry = (waveform == "RAMP");
    const bool showPulse = (waveform == "PULSE");
    const bool showNoise = (waveform == "NOISE");
    const bool showDC = (waveform == "DC");
    const bool showStandardControls = !showNoise && !showDC;

    m_frequencyLabel->setVisible(showStandardControls);
    m_frequencyEdit->setVisible(showStandardControls);

    m_amplitudeLabel->setVisible(showStandardControls);
    m_amplitudeEdit->setVisible(showStandardControls);

    m_offsetLabel->setVisible(showStandardControls);
    m_offsetEdit->setVisible(showStandardControls);

    m_phaseLabel->setVisible(showStandardControls);
    m_phaseEdit->setVisible(showStandardControls);

    m_dutyLabel->setVisible(showSquare);
    m_dutyEdit->setVisible(showSquare);

    m_rampSymmetryLabel->setVisible(showSymmetry);
    m_rampSymmetryEdit->setVisible(showSymmetry);

    m_pulseWidthLabel->setVisible(showPulse);
    m_pulseWidthEdit->setVisible(showPulse);

    m_pulseRiseLabel->setVisible(showPulse);
    m_pulseRiseEdit->setVisible(showPulse);

    m_pulseFallLabel->setVisible(showPulse);
    m_pulseFallEdit->setVisible(showPulse);

    m_pulseDutyLabel->setVisible(showPulse);
    m_pulseDutyEdit->setVisible(showPulse);

    const bool showNoiseBandwidth =
        showNoise && m_noiseBandsetCheck->isChecked();

    m_noiseBandsetLabel->setVisible(showNoise);
    m_noiseBandsetCheck->setVisible(showNoise);

    m_noiseStdevLabel->setVisible(showNoise);
    m_noiseStdevEdit->setVisible(showNoise);

    m_noiseMeanLabel->setVisible(showNoise);
    m_noiseMeanEdit->setVisible(showNoise);

    m_noiseBandwidthLabel->setVisible(showNoiseBandwidth);
    m_noiseBandwidthEdit->setVisible(showNoiseBandwidth);

    m_dcOffsetLabel->setVisible(showDC);
    m_dcOffsetEdit->setVisible(showDC);

    m_dcPrecisionHighLabel->setVisible(showDC);
    m_dcPrecisionHighCheck->setVisible(showDC);
}

void ChannelWidget::setControlsEnabled(bool enabled)
{
    m_groupBox->setEnabled(enabled);
}

void ChannelWidget::visitAllQuantityEdits(
    const std::function<void(QuantityEdit *)> &visitor)
{
    visitor(m_frequencyEdit);
    visitor(m_periodEdit);
    visitor(m_amplitudeEdit);
    visitor(m_offsetEdit);
    visitor(m_phaseEdit);
    visitor(m_dutyEdit);
    visitor(m_rampSymmetryEdit);
    visitor(m_pulseWidthEdit);
    visitor(m_pulseRiseEdit);
    visitor(m_pulseFallEdit);
    visitor(m_pulseDutyEdit);
    visitor(m_noiseStdevEdit);
    visitor(m_noiseMeanEdit);
    visitor(m_noiseBandwidthEdit);
    visitor(m_dcOffsetEdit);
}

void ChannelWidget::selectAllIfDirty(bool enabled)
{
    sdgDebug() << Q_FUNC_INFO;

    visitAllQuantityEdits(
        [enabled](QuantityEdit *edit)
        {
	    if (enabled)
                edit->selectIfDirty();
	    else
                edit->deselectIfDirty();
        });
}

void ChannelWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *showModified =
        menu.addAction("Show all modified fields");

    QAction *clearModified =
        menu.addAction("Clear modified-field highlighting");

    const QAction *action = menu.exec(event->globalPos());

    if (action == showModified)
    {
        visitAllQuantityEdits(
            [](QuantityEdit *edit)
            {
                edit->selectIfDirty();
            });
    }
    else if (action == clearModified)
    {
        visitAllQuantityEdits(
            [](QuantityEdit *edit)
            {
                edit->deselectIfDirty();
            });
    }
}

void ChannelWidget::debugLayout() const
{
    sdgDebug()
        << "CH" << m_channel
        << "widget" << size()
        << "hint" << sizeHint()
        << "minHint" << minimumSizeHint()
        << "group" << m_groupBox->size()
        << "groupHint" << m_groupBox->sizeHint()
        << "ampEdit" << m_amplitudeEdit->size()
        << "ampEditHint" << m_amplitudeEdit->sizeHint()
        << "ampMinHint" << m_amplitudeEdit->minimumSizeHint();
}
