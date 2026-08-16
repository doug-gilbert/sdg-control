#pragma once

#include <QString>

#include "ScpiConnection.h"
#include "Instrument.h"


class SDG2000X : public Instrument
{
    Q_OBJECT

public:
    explicit SDG2000X(QObject *parent = nullptr);

    ~SDG2000X() override;

    bool connectTo(const QString& ip) override;

    void disconnect() override;

    bool isConnected() const override;

    QString identification() override;

    QString getConnectionError() const override;

    ChannelState getChannelState(int channel) override;

    bool applyChannelState(int channel,
                           const ChannelState& state) override;

    bool setWaveform(int channel, const QString& waveform);
    bool setFrequency(int channel, double hz);
    bool setAmplitude(int channel, double volts);     // sets Vpp
    bool setAmplitudeVrms(int channel, double volts);
    bool setAmplitudedBm(int channel, double dbm);
    bool setUserAmplitude(int channel, const SdgAmplitude &amplitude);
    bool setOffset(int channel, double volts);
    bool setPhase(int channel, double degrees);
    bool setRampSymmetry(int channel, double percent);
    bool setPulseWidth(int channel, double seconds);
    bool setPulseRise(int channel, double seconds);
    bool setPulseFall(int channel, double seconds);
    bool setNoiseBandset(int channel, bool enabled);
    bool setNoiseStdev(int channel, double stdev);
    bool setNoiseMean(int channel, double mean);
    bool setNoiseBandwidth(int channel, double freq);
    bool setDcOffset(int channel, double value);
    bool setDcPrecisionHigh(int channel, bool enabled);

    bool output(int channel, bool enabled);

    // Siglent front-panel screen capture and virtual CH1/CH2 button press
    QByteArray getFrontPanelImage();
    bool toggleChannelFocus();

    bool hasFrontPanel() const override;

    bool clearErrors();

    bool getOutputState(int channel);

    bool waitForOperationComplete(int timeout_ms);

    QString getError();

private:

    ScpiConnection scpi;

    QString channelPrefix(int channel);
};
