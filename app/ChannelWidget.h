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
class AmplitudeState;
class OutputState;


class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(AppController *controller, int channel,
                           QWidget *parent = nullptr);

    void setStatus(const QString &text);   // visible if SDG_DEVELOPER_UI=ON

    // Going from internal state (where Units may be normalized) to UI
    void setWaveformState(const QString &waveform, bool makeDirty = true);
    void setFrequencyState(double value, bool makeDirty = true);
    void setAmplitudeState(const AmplitudeState &amplitud,
                           bool makeDirty = true);
    void setOffsetState(double value, bool makeDirty = true);
    void setPhaseState(double value, bool makeDirty = true);
    void setDutyState(double value, bool makeDirty = true);
    void setRampSymmetryState(double value, bool makeDirty = true);
    void setPulseWidthState(double value, bool makeDirty = true);
    void setPulseRiseState(double value, bool makeDirty = true);
    void setPulseFallState(double value, bool makeDirty = true);
    void setNoiseBandsetState(bool enabled, bool makeDirty = true);
    void setNoiseStdevState(double value, bool makeDirty = true);
    void setNoiseMeanState(double value, bool makeDirty = true);
    void setNoiseBandwidthState(double value, bool makeDirty = true);
    void setDcOffsetState(double value, bool makeDirty = true);
    void setDcPrecisionHighState(bool enabled, bool makeDirty = true);

    void setOutputState(const OutputState &output, bool makeDirty = true);

    void setControlsEnabled(bool enabled);

    void visitAllQuantityEdits(
        const std::function<void(QuantityEdit *)> &visitor);

    void clearAllDirty();

    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void waveformChanged(int channel, const QString &waveform);
    void frequencyChanged(int channel, double value);
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

    int channel;

    AppController *m_controller = nullptr;

    QScrollArea *scrollArea = nullptr;
    QWidget *scrollContents = nullptr;

    QGroupBox *groupBox;
    QFormLayout *formLayout;

#ifdef SDG_DEVELOPER_UI
    QLabel *statusLabel;
#endif

    QLabel *waveformLabel;
    QLabel *frequencyLabel;
    QLabel *periodLabel;
    QLabel *amplitudeLabel;
    QLabel *offsetLabel;
    QLabel *phaseLabel;
    QLabel *dutyLabel;
    QLabel *rampSymmetryLabel;
    QLabel *pulseWidthLabel;
    QLabel *pulseRiseLabel;
    QLabel *pulseFallLabel;
    QLabel *pulseDutyLabel;
    QLabel *noiseStdevLabel;
    QLabel *noiseMeanLabel;
    QLabel *noiseBandwidthLabel;
    QLabel *noiseBandsetLabel;
    QLabel *dcOffsetLabel;
    QLabel *dcPrecisionHighLabel;

    QComboBox *waveformCombo;
    QuantityEdit *frequencyEdit = nullptr;
    QuantityEdit *periodEdit = nullptr;
    QuantityEdit *amplitudeEdit = nullptr;
    QuantityEdit *offsetEdit = nullptr;
    QuantityEdit *phaseSpin = nullptr;
    QDoubleSpinBox *dutySpin;
    QDoubleSpinBox *rampSymmetrySpin;
    QDoubleSpinBox *pulseWidthSpin;
    QDoubleSpinBox *pulseRiseSpin;
    QDoubleSpinBox *pulseFallSpin;
    QDoubleSpinBox *pulseDutySpin;
    QCheckBox *noiseBandsetCheck;

    QDoubleSpinBox *noiseStdevSpin;
    QDoubleSpinBox *noiseMeanSpin;
    QDoubleSpinBox *noiseBandwidthSpin;

    QDoubleSpinBox *dcOffsetSpin;
    QCheckBox *dcPrecisionHighCheck;

    QCheckBox *outputCheck;

    QPushButton *closeButton = nullptr;
};
