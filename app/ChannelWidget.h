#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QCheckBox;
class QDoubleSpinBox;
class QComboBox;

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(int channel, QWidget *parent = nullptr);

    void setWaveform(const QString &waveform);
    void setFrequency(double value);
    void setAmplitude(double value);
    void setOffset(double value);
    void setOutput(bool enabled);
    void setStatus(const QString &text);

signals:
    void outputChanged(int channel, bool enabled);
    void frequencyChanged(int channel, double value);
    void amplitudeChanged(int channel, double value);
    void offsetChanged(int channel, double value);
    void waveformChanged(int channel, const QString &waveform);

private:
    int channel;

    QLabel *label;
    QCheckBox *outputCheck;

    QComboBox *waveformCombo;
    QDoubleSpinBox *frequencySpin;
    QDoubleSpinBox *amplitudeSpin;
    QDoubleSpinBox *offsetSpin;
};
