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

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(int channel, QWidget *parent = nullptr);

    void setWaveform(const QString &waveform);
    void setFrequency(double value);
    void setAmplitude(double value);
    void setOffset(double value);
    void setPhase(double value);
    void setSymmetry(double value);
    void setOutput(bool enabled);
    void setStatus(const QString &text);

signals:
    void outputChanged(int channel, bool enabled);
    void frequencyChanged(int channel, double value);
    void amplitudeChanged(int channel, double value);
    void offsetChanged(int channel, double value);
    void waveformChanged(int channel, const QString &waveform);
    void phaseChanged(int channel, double phase);
    void symmetryChanged(int channel, double percent);

private:
    void updateControlVisibility();

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
    QLabel *symmetryLabel;

    QCheckBox *outputCheck;

    QComboBox *waveformCombo;
    QDoubleSpinBox *frequencySpin;
    QDoubleSpinBox *amplitudeSpin;
    QDoubleSpinBox *offsetSpin;
    QDoubleSpinBox *phaseSpin;
    QDoubleSpinBox *symmetrySpin;
};
