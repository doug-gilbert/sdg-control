#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <QWidget>
#include <QString>

class QLabel;
class QCheckBox;
class QDoubleSpinBox;
class QComboBox;
class QGroupBox;
class QFormLayout;
class QPushButton;

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(int channel, QWidget *parent = nullptr);

    void setStatus(const QString &text);   // visible if SDG_DEVELOPER_UI=ON

    void setWaveform(const QString &waveform);
    void setFrequency(double value);
    void setAmplitude(double value);
    void setOffset(double value);
    void setPhase(double value);
    void setRampSymmetry(double value);
    void setPulseWidth(double value);
    void setPulseRise(double value);
    void setPulseFall(double value);
    void setNoiseBandset(bool enabled);
    void setNoiseStdev(double value);
    void setNoiseMean(double value);
    void setNoiseBandwidth(double value);
    void setDcOffset(double value);
    void setDcPrecisionHigh(bool enabled);

    void setOutput(bool enabled);

    void setControlsEnabled(bool enabled);

signals:
    void frequencyChanged(int channel, double value);
    void amplitudeChanged(int channel, double value);
    void offsetChanged(int channel, double value);
    void waveformChanged(int channel, const QString &waveform);
    void phaseChanged(int channel, double phase);
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

    int channel;

    QGroupBox *groupBox;
    QFormLayout *formLayout;

#ifdef SDG_DEVELOPER_UI
    QLabel *statusLabel;
#endif

    QLabel *waveformLabel;
    QLabel *frequencyLabel;
    QLabel *amplitudeLabel;
    QLabel *offsetLabel;
    QLabel *phaseLabel;
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
    QDoubleSpinBox *frequencySpin;
    QDoubleSpinBox *amplitudeSpin;
    QDoubleSpinBox *offsetSpin;
    QDoubleSpinBox *phaseSpin;
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
