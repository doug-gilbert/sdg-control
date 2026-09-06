#pragma once

#include <QString>

#include <optional>

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

    bool waitForOperationComplete(int timeout_ms) override;

    ChannelState getChannelState(int channel) override;

    bool applyChannelState(int channel,
                           const ChannelState& state) override;

    // Siglent front-panel screen capture and virtual CH1/CH2 button press
    QByteArray getFrontPanelImage() override;
    bool toggleChannelFocus() override;

    bool hasFrontPanel() const override;

    // reset and set default values on the unit
    bool reset() override;

    bool setSdgWaveform(int channel, const QString& waveform);
    bool setSdgFrequency(int channel, double hz);
    bool setSdgAmplitude(int channel, const AmplitudeState &amplitude);
    bool setSdgOffset(int channel, double volts);
    bool setSdgPhase(int channel, double degrees);
    bool setSdgRampSymmetry(int channel, double percent);
    bool setSdgPulseWidth(int channel, double seconds);
    bool setSdgPulseRise(int channel, double seconds);
    bool setSdgPulseFall(int channel, double seconds);
    bool setSdgNoiseBandset(int channel, bool enabled);
    bool setSdgNoiseStdev(int channel, double stdev);
    bool setSdgNoiseMean(int channel, double mean);
    bool setSdgNoiseBandwidth(int channel, double freq);
    bool setSdgDcOffset(int channel, double value);
    bool setSdgDcPrecisionHigh(int channel, bool enabled);
    bool setSdgDuty(int channel, double percent);

    bool setSdgOutput(int channel, bool enabled);

    bool setSdgOutputLoadPol(int channel, bool enabled, bool load50,
                             bool polNormal);

    bool setSdgOutputBoth(bool enabled);

    bool invert(int channel, bool enabled);

    bool clearErrors();

    std::optional<OutputState> getOutputState(int channel);

    QString getError();

private:

    ScpiConnection scpi;

    QString channelPrefix(int channel);
};
