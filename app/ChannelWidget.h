/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QWidget>
#include <QString>

#include <functional>

#include "ChannelState.h"


class QLabel;
class QCheckBox;
class QDoubleSpinBox;
class QComboBox;
class QGroupBox;
class QFormLayout;
class QPushButton;
class QScrollArea;

class StepAdjustSpinBox;
class QuantityEdit;
class AppController;
struct AmplitudeState;
struct OutputState;
struct ChannelDirtyState;


/// The SDG2000X series are all two channel devices that are relatively
/// independent. Each channel is modelled by an instance of this class.
/// Other settings that are common to both channels (e.g. Clock source)
/// are modelled by an instance of the GeneralWidget class.
/// MainWindow is the parent of this class.
class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(AppController *controller, int channel,
                           const ChannelDirtyState *dirtyState,
                           QWidget *parent = nullptr);

    ~ChannelWidget();

    void setUiStatus(const QString &text);   // visible if SDG_DEVELOPER_UI=ON

    void setVisibleUiStatus(bool enable);

    // Going from internal state (where Units may be normalized) to UI
    void setUiWaveform(const QString &waveform);
    void setUiFrequency(double value);
    void setUiAmplitude(const AmplitudeState &amplitude);
    void setUiOffset(double value);
    void setUiVHigh(double value);
    void setUiVLow(double value);
    void setUiPhase(double value);
    void setUiDuty(double value);
    void setUiRampSymmetry(double value);
    void setUiPulseWidth(double value);
    void setUiPulseRise(double value);
    void setUiPulseFall(double value);
    void setUiNoiseBandset(bool enabled);
    void setUiNoiseStdev(double value);
    void setUiNoiseMean(double value);
    void setUiNoiseBandwidth(double value);
    void setUiDcOffset(double value);
    void setUiDcPrecisionHigh(bool enabled);

    // Note: there are multiple UI fields within OutputState
    void setUiOutput(const OutputState &output);

    void setControlsEnabled(bool enabled);

    void visitAllQuantityEdits(
        const std::function<void(QuantityEdit *)> &visitor);

    // If enabled is true, the contents of each field is selected
    // (highlighted) if the corresponding dirty flag is set. For QuantityEdit
    // based numeric fields, only the the numeric part is selected; for
    // ComboBox only fields, the "dirty" selection is highlighted. If enabled
    // is false, and if the corresponding dirty flag is set, then the
    // contents of each field is deselected. So selectAllIfDirty(false)
    // undoes what selectAllIfDirty(true) does.
    void selectAllIfDirty(bool enabled);

    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void waveformChanged(int channel, const QString &waveform);
    void frequencyChanged(int channel, double value);
    void periodChanged(int channel, double value);
    void amplitudeChanged(int channel, double value,
                          const QString &representation);
    void offsetChanged(int channel, double value,
                       const QString &representation);
    void vHighChanged(int channel, double value,
                      const QString &representation);
    void vLowChanged(int channel, double value,
                     const QString &representation);
    void phaseChanged(int channel, double phase);
    void dutyChanged(int channel, double duty);
    void rampSymmetryChanged(int channel, double percent);
    void pulseWidthChanged(int channel, double value);
    void pulseRiseChanged(int channel, double value);
    void pulseFallChanged(int channel, double value);
    void noiseBandsetChanged(int channel, bool enabled);
    void noiseStdevChanged(int channel, double value);
    void noiseMeanChanged(int channel, double value);
    void noiseBandwidthChanged(int channel, double value);
    void dcOffsetChanged(int channel, double value);
    void dcPrecisionHighChanged(int channel, bool enabled);
    void polarityChanged(int channel, Polarity polarity);
    void outputLoadChanged(int channel, OutputLoad load);
    void externalOutputChanged(int channel, bool enabled);

    void hideRequested(int channel);

private:
    void updateControlVisibility();
    void updatePulseDuty();

    // constructor helpers
    void prepareFrequencyPeriod(bool preLayout);
    void prepareAmplitudeOffset(bool preLayout);
    void prepareVHighLow(bool preLayout);

    void debugLayout() const;

    int m_channel;

    const ChannelDirtyState *m_dirtyState;

    QPushButton *m_closeButton = nullptr;

    AppController *m_controller = nullptr;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContents = nullptr;

    QGroupBox *m_groupBox;
    QFormLayout *m_formLayout;

#ifdef SDG_DEVELOPER_UI
    QLabel *m_statusLabel;
    QLabel *m_statusIntroLabel;
#endif

    QLabel *m_waveformLabel;
    QLabel *m_frequencyLabel;
    QLabel *m_periodLabel;
    QLabel *m_amplitudeLabel;
    QLabel *m_offsetLabel;
    QLabel *m_vHighLabel;
    QLabel *m_vLowLabel;
    QLabel *m_phaseLabel;
    QLabel *m_dutyLabel;
    QLabel *m_rampSymmetryLabel;
    QLabel *m_pulseWidthLabel;
    QLabel *m_pulseRiseLabel;
    QLabel *m_pulseFallLabel;
    QLabel *m_pulseDutyLabel;
    QLabel *m_noiseStdevLabel;
    QLabel *m_noiseMeanLabel;
    QLabel *m_noiseBandwidthLabel;
    QLabel *m_noiseBandsetLabel;
    QLabel *m_dcOffsetLabel;
    QLabel *m_dcPrecisionHighLabel;
    QLabel *m_polarityLabel;
    QLabel *m_outputLoadLabel;

    QComboBox *m_waveformCombo;
    QuantityEdit *m_frequencyEdit = nullptr;
    QuantityEdit *m_periodEdit = nullptr;
    QuantityEdit *m_amplitudeEdit = nullptr;
    QuantityEdit *m_offsetEdit = nullptr;
    QuantityEdit *m_vHighEdit = nullptr;
    QuantityEdit *m_vLowEdit = nullptr;
    QuantityEdit *m_phaseEdit = nullptr;
    QuantityEdit *m_dutyEdit = nullptr;
    QuantityEdit *m_rampSymmetryEdit = nullptr;
    QuantityEdit *m_pulseWidthEdit = nullptr;
    QuantityEdit *m_pulseRiseEdit = nullptr;
    QuantityEdit *m_pulseFallEdit = nullptr;
    QuantityEdit *m_pulseDutyEdit = nullptr;
    QCheckBox *m_noiseBandsetCheck;
    QuantityEdit *m_noiseStdevEdit;
    QuantityEdit *m_noiseMeanEdit;
    QuantityEdit *m_noiseBandwidthEdit;
    QuantityEdit *m_dcOffsetEdit;
    QCheckBox *m_dcPrecisionHighCheck;
    QComboBox *m_polarityCombo;
    QComboBox *m_outputLoadCombo;
    QCheckBox *m_externalOutputCheck;

    enum class FrequencyPeriodMode
    {
        Frequency,
        Period,
        Both
    };

    enum class AmplitudeMode
    {
        Normal,
        HighLow
    };

    void setFrequencyPeriodMode(FrequencyPeriodMode mode);
    void setAmplitudeMode(AmplitudeMode mode);

    FrequencyPeriodMode m_frequencyPeriodMode =
        FrequencyPeriodMode::Frequency;

    AmplitudeMode m_amplitudeMode = AmplitudeMode::Normal;
};
