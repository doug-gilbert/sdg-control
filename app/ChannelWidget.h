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


class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(AppController *controller, int channel,
                           QWidget *parent = nullptr);

    void setUiStatus(const QString &text);   // visible if SDG_DEVELOPER_UI=ON

    // Going from internal state (where Units may be normalized) to UI
    void setUiWaveform(const QString &waveform);
    void setUiFrequency(double value);
    void setUiAmplitude(const AmplitudeState &amplitud);
    void setUiOffset(double value);
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

    void setUiOutput(const OutputState &output);

    void setControlsEnabled(bool enabled);

    void visitAllQuantityEdits(
        const std::function<void(QuantityEdit *)> &visitor);

    void clearAllDirty();

    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void waveformChanged(int channel, const QString &waveform);
    void frequencyChanged(int channel, double value);
    void periodChanged(int channel, double value);
    void amplitudeChanged(int channel, double value,
                          const QString &representation);
    void offsetChanged(int channel, double value,
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

    void outputChanged(int channel, bool enabled);

    void hideRequested(int channel);

private:
    void updateControlVisibility();
    void updatePulseDuty();

    void debugLayout() const;

    int m_channel;

    QPushButton *m_closeButton = nullptr;

    AppController *m_controller = nullptr;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContents = nullptr;

    QGroupBox *m_groupBox;
    QFormLayout *m_formLayout;

#ifdef SDG_DEVELOPER_UI
    QLabel *m_statusLabel;
#endif

    QLabel *m_waveformLabel;
    QLabel *m_frequencyLabel;
    QLabel *m_periodLabel;
    QLabel *m_amplitudeLabel;
    QLabel *m_offsetLabel;
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

    QComboBox *m_waveformCombo;
    QuantityEdit *m_frequencyEdit = nullptr;
    QuantityEdit *m_periodEdit = nullptr;
    QuantityEdit *m_amplitudeEdit = nullptr;
    QuantityEdit *m_offsetEdit = nullptr;
    QuantityEdit *m_phaseEdit = nullptr;
    QuantityEdit *m_dutyEdit = nullptr;
    QuantityEdit *m_rampSymmetryEdit = nullptr;
    QuantityEdit *m_pulseWidthEdit = nullptr;
    QuantityEdit *m_pulseRiseEdit = nullptr;
    QuantityEdit *m_pulseFallEdit = nullptr;
    QuantityEdit *m_pulseDutyEdit = nullptr;
    QCheckBox *m_noiseBandsetCheck;
    QDoubleSpinBox *m_noiseStdevSpin;
    QDoubleSpinBox *m_noiseMeanSpin;
    QDoubleSpinBox *m_noiseBandwidthSpin;
    QDoubleSpinBox *m_dcOffsetSpin;
    QCheckBox *m_dcPrecisionHighCheck;
    QCheckBox *m_outputCheck;

    friend class MainWindow;    // allow access to each field's dirty flag

    // Accessors for fields based on QuantityEdit.
    // MainWindow is the only intended caller.
    QuantityEdit *frequencyEdit() const { return m_frequencyEdit; }
    QuantityEdit *periodEdit() const { return m_periodEdit; }
    QuantityEdit *amplitudeEdit() const { return m_amplitudeEdit; }
    QuantityEdit *offsetEdit() const { return m_offsetEdit; }
    QuantityEdit *phaseEdit() const { return m_phaseEdit; }
    QuantityEdit *dutyEdit() const { return m_dutyEdit; }

    // Not QuantityEdit based yet
    QuantityEdit *waveformEdit() const { return nullptr; }
    QuantityEdit *rampSymmetryEdit() const { return nullptr; }
    QuantityEdit *pulseWidthEdit() const { return nullptr; }
    QuantityEdit *pulseRiseEdit() const { return nullptr; }
    QuantityEdit *pulseFallEdit() const { return nullptr; }
    QuantityEdit *pulseDutyEdit() const { return nullptr; }
    QuantityEdit *noiseBandsetEdit() const { return nullptr; }
    QuantityEdit *noiseStdevEdit() const { return nullptr; }
    QuantityEdit *noiseMeanEdit() const { return nullptr; }
    QuantityEdit *noiseBandwidthEdit() const { return nullptr; }
    QuantityEdit *dcOffsetEdit() const { return nullptr; }
    QuantityEdit *dcPrecisionHighEdit() const { return nullptr; }
    QuantityEdit *outputEdit() const { return nullptr; }
};
