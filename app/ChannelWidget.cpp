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
#include <QLineEdit>

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
#include "AppHelper.h"
#include "Utility.h"
#include "debug.h"


namespace       // anonymous namespace so all within are at file scope
{

void frequency2PeriodSwitch(const QuantityEdit * qe, int flag)
{
    DEBUG_FUNC << qe->objectName();
    emit qe->frequencySwap(flag);
}

void period2FrequencySwitch(const QuantityEdit * qe, int flag)
{
    DEBUG_FUNC << qe->objectName();
    emit qe->periodSwap(flag);
}

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
            {"uHz", "Hz", 0.000'001},

            {"Switch to period", "", 0.0, frequency2PeriodSwitch, 0},
            {"Add period field", "", 0.0, frequency2PeriodSwitch, 1}
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
            {"ns", "s",  0.000'000'001},

            {"Switch to frequency", "", 0.0, period2FrequencySwitch, 0},
            {"Add frequency field", "", 0.0, period2FrequencySwitch, 1}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "s" };
    }
};

const FrequencyRepresentation frequencyQuantityRepresentation;
const PeriodRepresentation periodQuantityRepresentation;


// Start of Amplitude section

void amplitudeModeSwitch(const QuantityEdit * qe, int flag)
{
    DEBUG_FUNC << qe->objectName();
    emit qe->amplitudeModeSwitch(flag);
}

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
            {"dBm",   "dBm",    1.0},

            {"Switch to Normal", "", 0.0, amplitudeModeSwitch, 0},
            {"Switch to Vhigh/Vlow", "", 0.0, amplitudeModeSwitch, 1}
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
            {"mVdc",  "Vdc",  0.001},

            {"Switch to Normal", "", 0.0, amplitudeModeSwitch, 0},
            {"Switch to Vhigh/Vlow", "", 0.0, amplitudeModeSwitch, 1}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "Vdc" };
    }
};

const OffsetRepresentation offsetRepresentation;


// Start of VHighLow section
class VHighLowRepresentation : public QuantityRepresentation
{
public:
    std::vector<QuantityRepresentation::Representation>
                                            representations() const override
    {
        return {
            {"V",   "V",    1.0},
            {"mV",  "V",    0.001},

            {"Switch to Normal", "", 0.0, amplitudeModeSwitch, 0},
            {"Switch to Vhigh/Vlow", "", 0.0, amplitudeModeSwitch, 1}
        };
    }

    QString canonicalRepresentation() const override
    {
        return { "V" };
    }
};

const VHighLowRepresentation vHighLowQuantityRepresentation;

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
        return {"°"};    /* that is a multi-byte UTF-8 character */
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

}       // <<<<< end of anonymous namespace >>>>>


/*
 * vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
 * The start of the main class this source file is named after
 */
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
        // Qt::ScrollBarAlwaysOff);
        Qt::ScrollBarAsNeeded);
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

    m_statusIntroLabel = new QLabel("Status:", m_groupBox);
    m_formLayout->addRow(m_statusIntroLabel, m_statusLabel);
#endif

    m_waveformCombo = new QComboBox(m_groupBox);

    m_waveformCombo->addItems({
        "SINE",
        "SQUARE",
        "RAMP",
        "PULSE",
        "NOISE",
        "DC",
        "ARB"
    });
    m_waveformCombo->setObjectName("waveformCombo");
    m_waveformCombo->setToolTip("Best to start with this field");

    prepareFrequencyPeriod(/* preLayout */ true);

    prepareAmplitudeOffset(/* preLayout */ true);

    prepareVHighLow(/* preLayout */ true);

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
    m_phaseEdit->setValueToolTip("Phase angle in degrees, from -360 to 360");

    m_dutyEdit = new QuantityEdit(m_controller, dutyRepresentation,
                                   m_dirtyState->m_duty, m_groupBox);
    m_dutyEdit->setObjectName("dutyEdit");
    // No comboBox so we can call setrange() directly
    m_dutyEdit->setRange(0.0, 100.0);
    m_dutyEdit->setDecimals(1);
    m_dutyEdit->setSingleStep(1.0);
    m_dutyEdit->setStepLimits(0.1, 10.0);
    m_dutyEdit->setValueToolTip(
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
    m_rampSymmetryEdit->setValueToolTip(
                           "(ramp_up / (ramp_up+ramp_down)) * 100");

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
    m_pulseRiseEdit->setDecimals(9);
    m_pulseRiseEdit->setSingleStep(0.000'001);

    m_pulseFallEdit = new QuantityEdit(m_controller,
                                       pulseFallRepresentation,
                                       m_dirtyState->m_pulseFall,
                                       m_groupBox);
    m_pulseFallEdit->setObjectName("pulseFallEdit");
    // m_pulseFallEdit->setRange(0.001, 1'000'000.0);
    m_pulseFallEdit->setDecimals(9);
    m_pulseFallEdit->setSingleStep(0.000'001);

    m_pulseDutyEdit = new QuantityEdit(m_controller, pulseDutyRepresentation,
          /* duty or pulseDuty ?? */   m_dirtyState->m_duty,
                                       m_groupBox);
    m_pulseDutyEdit->setObjectName("pulseDutyEdit");
    // No comboBox so we can call setrange() directly
    m_pulseDutyEdit->setRange(0.0, 100.0);
    m_pulseDutyEdit->setDecimals(1);
    m_pulseDutyEdit->setSingleStep(1.0);
    m_pulseDutyEdit->setStepLimits(0.1, 10.0);
    m_pulseDutyEdit->setValueToolTip(
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


    m_polarityCombo = new QComboBox(m_groupBox);
    m_polarityCombo->addItem("Normal",
                             QVariant::fromValue(Polarity::Normal));
    m_polarityCombo->addItem("Inverted",
                             QVariant::fromValue(Polarity::Inverted));
    m_polarityCombo->setObjectName("polarityCombo");

    m_outputLoadCombo = new QComboBox(m_groupBox);
    m_outputLoadCombo->addItem("50",
                               QVariant::fromValue(OutputLoad::Ohms50));
    m_outputLoadCombo->addItem("HiZ",    // for 'High impedance'
                               QVariant::fromValue(OutputLoad::HiZ));
    m_outputLoadCombo->setObjectName("outputLoadCombo");

    m_externalOutputCheck = new QCheckBox(chOutStr, m_groupBox);
    m_externalOutputCheck->setObjectName("externalOutputCheck");

    // Create widgets and labels
    m_waveformLabel = new QLabel("Waveform:", m_groupBox);
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
    m_polarityLabel = new QLabel("Polarity:", m_groupBox);
    m_outputLoadLabel = new QLabel("Load impedance:", m_groupBox);

    updateControlVisibility();

    // Add labels and related fields to the form (which is in a groupbox)
    // The ORDER fields appear in the UI is dictated by the following calls.
    m_formLayout->addRow(m_waveformLabel, m_waveformCombo);
    m_formLayout->addRow(m_frequencyLabel, m_frequencyEdit);
    m_formLayout->addRow(m_periodLabel, m_periodEdit);
    m_formLayout->addRow(m_amplitudeLabel, m_amplitudeEdit);
    m_formLayout->addRow(m_offsetLabel, m_offsetEdit);
    m_formLayout->addRow(m_vHighLabel, m_vHighEdit);
    m_formLayout->addRow(m_vLowLabel, m_vLowEdit);
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

    auto *rowLayout = new QHBoxLayout;
    rowLayout->addWidget(m_polarityLabel);
    rowLayout->addWidget(m_polarityCombo);
    rowLayout->addSpacing(15);
    rowLayout->addWidget(m_outputLoadLabel);
    rowLayout->addWidget(m_outputLoadCombo);
    m_formLayout->addRow(rowLayout);

    m_formLayout->addRow(m_externalOutputCheck);

    // updateControlVisibility() call is _after_ the connect() calls

    connect(m_waveformCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &waveform)
            {
                updateControlVisibility();
                emit waveformChanged(this->m_channel, waveform);
            });

    prepareFrequencyPeriod(/* preLayout */ false);

    prepareAmplitudeOffset(/* preLayout */ false);

    prepareVHighLow(/* preLayout */ false);

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

    connect(m_polarityCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &)
            {
                const auto polarity = m_polarityCombo->currentIndex() == 0
                                      ? Polarity::Normal : Polarity::Inverted;

                emit polarityChanged(this->m_channel, polarity);
            });

    connect(m_outputLoadCombo,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &)
            {
                const auto load = m_outputLoadCombo->currentIndex() == 0
                                  ? OutputLoad::Ohms50 : OutputLoad::HiZ;

                emit outputLoadChanged(this->m_channel, load);
            });


    connect(m_externalOutputCheck,
            &QCheckBox::toggled,
            this,
            [this](bool enabled)
            {
                emit externalOutputChanged(this->m_channel, enabled);
            });

    connect(m_closeButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                sdgDebug() << "Close clicked for channel" << m_channel;
                emit hideRequested(m_channel);
            });

    // This sets initial visibility (whether or not fields are shown)
    updateControlVisibility();

}

ChannelWidget::~ChannelWidget()
{
    DEBUG_FUNC << "Channel:" << m_channel;
}

/*
 * These prepare*() methods offload boilerplate code from the constructor.
 * Previously code for each field (or pair of related fields) was spread
 * across are a 600 line (plus) constructor.
 * These methods all assume that the prepare*(true) will be called BEFORE
 * the corresponding prepare*(false); if not a Q_ASSERT_X will be tripped.
 */
void ChannelWidget::prepareFrequencyPeriod(bool preLayout)
{
    QuantityEdit * qe;

    if (preLayout)
    {
        m_frequencyEdit = new QuantityEdit(m_controller,
                                           frequencyQuantityRepresentation,
                                           m_dirtyState->m_frequency,
                                           m_groupBox);
        qe = m_frequencyEdit;
        qe->setObjectName("frequencyEdit");
        qe->setValueToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");
        qe->setUnitToolTip("Right click here for more options");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Fixed);
        qe->setCanonicalRange(0.000'01, 120'000'000);
        qe->setDecimals(6);
        qe->setSingleStep(0.000'01);
        qe->setStepLimits(0.000'01, 100'000'000.0);
        qe->setValue(1'000.0, "Hz");
        m_frequencyLabel = new QLabel("Frequency:", m_groupBox);

        m_periodEdit = new QuantityEdit(m_controller,
                                        periodQuantityRepresentation,
              /* not an error --> */    m_dirtyState->m_frequency,
                                        m_groupBox);
        qe = m_periodEdit;
        qe->setObjectName("periodEdit");
        qe->setValueToolTip(
        "Right click in the numeric field to modify\n"
        "the spinner step size");
        qe->setUnitToolTip("Right click here for more options");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
        qe->setCanonicalRange(0.000'000'008'3, 1'000'000.0);
        qe->setDecimals(6);
        qe->setSingleStep(0.000'000'001);
        qe->setStepLimits(0.000'000'000'001, 1'000'000.0);
        qe->setValue(0.001, "s");
        m_periodLabel = new QLabel("Period:", m_groupBox);
    }
    else
    {         // after the formLayout invocations we set up the connects
        qe = m_frequencyEdit;
        Q_ASSERT_X(qe, __func__, "m_frequencyEdit=nullptr, bad ordering?");

        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &,
                       const QuantityEdit::Value &final)
                {
                    Q_UNUSED(final);

                    const double frequency = m_frequencyEdit->canonicalValue();

                    if (frequency > 0.0) {
                        const double period = 1.0 / frequency;

                        m_periodEdit->setValue(period, "s");

                        DEBUG_FUNC << "frequency=" << frequency << "Hz"
                                   << "period=" << period << "s";
                    }
                    else {
                        DEBUG_FUNC << "<< WILD frequency="
                                   << frequency << "Hz >>";
                    }

                    updatePulseDuty();
                    emit frequencyChanged(this->m_channel, frequency);
                });

        connect(qe, &QuantityEdit::frequencySwap,
                this,
                [this](int flag)
                {
                    if (flag == 0)
                        setFrequencyPeriodMode(FrequencyPeriodMode::Period);
                    else
                        setFrequencyPeriodMode(FrequencyPeriodMode::Both);
                });


        qe = m_periodEdit;
        Q_ASSERT_X(qe, __func__, "m_periodEdit=nullptr, bad ordering?");

        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &,
                       const QuantityEdit::Value &final)
                {
                    Q_UNUSED(final);

                    const double period = m_periodEdit->canonicalValue();

                    if (period > 0.0) {
                        const double frequency = 1.0 / period;

                        m_frequencyEdit->setValue(frequency, "Hz");

                        DEBUG_FUNC << "period=" << period << "s"
                                   << "frequency=" << frequency << "Hz";
                    }
                    else {
                        DEBUG_FUNC <<  "<< WILD period=" << period << "s >>";
                    }

                    updatePulseDuty();
                    emit periodChanged(this->m_channel, period);
                });

        connect(qe, &QuantityEdit::periodSwap,
                this,
                [this](int flag)
                {
                    if (flag == 0)
                        setFrequencyPeriodMode(FrequencyPeriodMode::Frequency);
                    else
                        setFrequencyPeriodMode(FrequencyPeriodMode::Both);
                });

    }
}

void ChannelWidget::prepareAmplitudeOffset(bool preLayout)
{
    QuantityEdit * qe;

    if (preLayout)
    {
        m_amplitudeEdit = new QuantityEdit(m_controller,
                                           amplitudeQuantityRepresentation,
                                           m_dirtyState->m_amplitude,
                                           m_groupBox);
        qe = m_amplitudeEdit;
        qe->setObjectName("amplitudeEdit");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        qe->setValueToolTip(
            "Right click in the numeric field to modify\n"
            "the spinner step size");
        qe->setUnitToolTip(
                         "Right click here for more voltage options");
        m_amplitudeLabel = new QLabel("Amplitude:", m_groupBox);

        m_offsetEdit = new QuantityEdit(m_controller,
                                        offsetRepresentation,
                                        m_dirtyState->m_offset,
                                        m_groupBox);
        qe = m_offsetEdit;
        qe->setObjectName("offsetEdit");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        qe->setValueToolTip(
            "Right click in the numeric field to modify\n"
            "the spinner step size");
        qe->setUnitToolTip(
                         "Right click here for more voltage options");
        m_offsetLabel = new QLabel("Offset:", m_groupBox);
    }
    else
    {         // after the formLayout invocations we set up the connects
        qe = m_amplitudeEdit;
        Q_ASSERT_X(qe, __func__, "m_amplitudeEdit=nullptr, bad ordering?");
        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &original,
                       const QuantityEdit::Value &final)
                {
                    sdgDebug() << "Amplitude field contents:"
                               << m_amplitudeEdit->cleanText();
                    qsdgDebug() << "amplitude committed:"
                                << "original =" << original.value
                                << original.representation
                                << "final =" << final.value
                                << final.representation;

                    emit amplitudeChanged(m_channel,
                                         final.value,
                                         final.representation);
                });

        connect(qe, &QuantityEdit::amplitudeModeSwitch,
                this,
                [this](int flag)
                {
                    if (flag == 0)
                        setAmplitudeMode(AmplitudeMode::Normal);
                    else
                        setAmplitudeMode(AmplitudeMode::HighLow);
                });

        qe = m_offsetEdit;
        Q_ASSERT_X(qe, __func__, "m_offsetEdit=nullptr, bad ordering?");
        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &,
                       const QuantityEdit::Value &final)
                {
                    sdgDebug() << m_offsetEdit->debugString();
                    sdgDebug() << objectName()
                        << "offset committed:"
                        << "value =" << final.value
                        << "representation =" << final.representation;

                    emit offsetChanged(m_channel, final.value,
                                       final.representation);
                });
    }
}

void ChannelWidget::prepareVHighLow(bool preLayout)
{
    QuantityEdit * qe;

    if (preLayout)
    {
        m_vHighEdit = new QuantityEdit(m_controller,
                                       vHighLowQuantityRepresentation,
                                       m_dirtyState->m_vHigh,
                                       m_groupBox);
        qe = m_vHighEdit;
        qe->setObjectName("vHighEdit");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        qe->setCanonicalRange(-10.0, 10.0);
        qe->setDecimals(4);
        qe->setSingleStep(0.001);
        qe->setStepLimits(0.000'1, 1.0);
        qe->setValue(0.001, "V");
        qe->setValueToolTip(
            "Right click in the numeric field to modify\n"
            "the spinner step size");
        qe->setUnitToolTip("Right click here for Amplitude options");
        m_vHighLabel = new QLabel("Voltage high:", m_groupBox);

        m_vLowEdit = new QuantityEdit(m_controller,
                                      vHighLowQuantityRepresentation,
                                      m_dirtyState->m_vLow,
                                      m_groupBox);
        qe = m_vLowEdit;
        qe->setObjectName("vLowEdit");
        qe->setMinimumWidth(215);
        qe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        qe->setCanonicalRange(-10.0, 10.0);
        qe->setDecimals(4);
        qe->setSingleStep(0.001);
        qe->setStepLimits(0.000'1, 1.0);
        qe->setValue(-0.001, "V");
        qe->setValueToolTip(
            "Right click in the numeric field to modify\n"
            "the spinner step size");
        qe->setUnitToolTip("Right click here for Amplitude options");
        m_vLowLabel = new QLabel("Voltage low:", m_groupBox);
    }
    else
    {         // after the formLayout invocations we set up the connects
        qe = m_vHighEdit;
        Q_ASSERT_X(qe, __func__, "m_vHighEdit=nullptr, bad ordering?");
        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &original,
                       const QuantityEdit::Value &final)
                {
                    Q_UNUSED(original);

                    emit vHighChanged(m_channel,
                                      final.value,
                                      final.representation);
                });

        connect(qe, &QuantityEdit::amplitudeModeSwitch,
                this,
                [this](int flag)
                {
                    if (flag == 0)
                        setAmplitudeMode(AmplitudeMode::Normal);
                    else
                        setAmplitudeMode(AmplitudeMode::HighLow);
                });

        qe = m_vLowEdit;
        Q_ASSERT_X(qe, __func__, "m_vLowEdit=nullptr, bad ordering?");
        connect(qe, &QuantityEdit::committed,
                this,
                [this](const QuantityEdit::Value &original,
                       const QuantityEdit::Value &final)
                {
                    Q_UNUSED(original);

                    emit vLowChanged(m_channel,
                                     final.value,
                                     final.representation);
                });

        connect(qe, &QuantityEdit::amplitudeModeSwitch,
                this,
                [this](int flag)
                {
                    if (flag == 0)
                        setAmplitudeMode(AmplitudeMode::Normal);
                    else
                        setAmplitudeMode(AmplitudeMode::HighLow);
                });
    }
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
        DEBUG_FUNC << "frequency=" << frequency << "Hz"
                   << "period=" << period << "s";
    }
    else
        DEBUG_FUNC << "<< WILD frequency=" << frequency << "Hz >>";
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
        DEBUG_FUNC << objectName() << "defaulting to Vpp";
        m_amplitudeEdit->setValue(amplit.getVpp(), "Vpp");
    }
    else
        DEBUG_FUNC << objectName() << ">>> BAD representation: " << rep;
}

void ChannelWidget::setUiOffset(double offset)
{
    m_offsetEdit->setValue(offset, "Vdc");
}

void ChannelWidget::setUiVHigh(double vLevel)
{
    m_vHighEdit->setValue(vLevel, "V");
}

void ChannelWidget::setUiVLow(double vLevel)
{
    m_vLowEdit->setValue(vLevel, "V");
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

// There are no setUiPolarity and setUiOutputLoad methods, see setUiOutput

void ChannelWidget::setUiOutput(const OutputState &output)
{
    m_externalOutputCheck->blockSignals(true);
    m_externalOutputCheck->setChecked(output.externalOutput);
    m_externalOutputCheck->blockSignals(false);

    m_polarityCombo->blockSignals(true);
    m_polarityCombo->setCurrentText(
                     Utility::polarityToString(output.polarity));
    m_polarityCombo->blockSignals(false);

    m_outputLoadCombo->blockSignals(true);
    m_outputLoadCombo->setCurrentText(
                       Utility::outputLoadToString(output.outputLoad));
    m_outputLoadCombo->blockSignals(false);
}

void ChannelWidget::setUiStatus(const QString &text)
{
#ifdef SDG_DEVELOPER_UI
    m_statusLabel->setText(text);
#else
    Q_UNUSED(text);
#endif
}

void ChannelWidget::setVisibleUiStatus(bool enable)
{
#ifdef SDG_DEVELOPER_UI
    m_statusIntroLabel->setVisible(enable);
    m_statusLabel->setVisible(enable);
#else
    Q_UNUSED(enable);
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

    const bool showFrequency =
        showStandardControls &&
        m_frequencyPeriodMode != FrequencyPeriodMode::Period;

    const bool showPeriod =
        showStandardControls &&
        m_frequencyPeriodMode != FrequencyPeriodMode::Frequency;

    m_frequencyLabel->setVisible(showFrequency);
    m_frequencyEdit->setVisible(showFrequency);

    m_periodLabel->setVisible(showPeriod);
    m_periodEdit->setVisible(showPeriod);

    const bool showNormalAmplitude =
        showStandardControls &&
        m_amplitudeMode == AmplitudeMode::Normal;

    const bool showHighLowAmplitude =
        showStandardControls &&
        m_amplitudeMode == AmplitudeMode::HighLow;

    m_amplitudeLabel->setVisible(showNormalAmplitude);
    m_amplitudeEdit->setVisible(showNormalAmplitude);

    m_offsetLabel->setVisible(showNormalAmplitude);
    m_offsetEdit->setVisible(showNormalAmplitude);

    m_vHighLabel->setVisible(showHighLowAmplitude);
    m_vHighEdit->setVisible(showHighLowAmplitude);

    m_vLowLabel->setVisible(showHighLowAmplitude);
    m_vLowEdit->setVisible(showHighLowAmplitude);

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
    visitor(m_vHighEdit);
    visitor(m_vLowEdit);
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
    DEBUG_FUNC << "Channel:" << m_channel;

    visitAllQuantityEdits(
        [enabled](QuantityEdit *qedit)
        {
            if (qedit)
            {
                if (enabled)
                    qedit->selectIfDirty();
                else
                    qedit->deselectIfDirty();
            }
        });

    // ComboBoxes and CheckBoxes
    if (enabled) {
        if (m_dirtyState->m_waveform)
            AppHelper::selectCombo(m_waveformCombo);
        if (m_dirtyState->m_polarity)
            AppHelper::selectCombo(m_polarityCombo);
        if (m_dirtyState->m_outputLoad)
            AppHelper::selectCombo(m_outputLoadCombo);
        if (m_dirtyState->m_externalOutput)
            AppHelper::selectCheckBox(m_externalOutputCheck);
    } else {
        AppHelper::deselectCombo(m_waveformCombo);
        AppHelper::deselectCombo(m_polarityCombo);
        AppHelper::deselectCombo(m_outputLoadCombo);
        AppHelper::deselectCheckBox(m_externalOutputCheck);
    }
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
        selectAllIfDirty(true);
#if 0
        visitAllQuantityEdits(
            [](QuantityEdit *edit)
            {
                edit->selectIfDirty();
            });
#endif
    }
    else if (action == clearModified)
    {
        selectAllIfDirty(false);
#if 0
        visitAllQuantityEdits(
            [](QuantityEdit *edit)
            {
                edit->deselectIfDirty();
            });
#endif
    }
}

void ChannelWidget::setFrequencyPeriodMode(FrequencyPeriodMode mode)
{
    m_frequencyPeriodMode = mode;
    updateControlVisibility();
}

void ChannelWidget::setAmplitudeMode(AmplitudeMode mode)
{
    m_amplitudeMode = mode;
    updateControlVisibility();
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
