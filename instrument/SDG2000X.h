#pragma once

#include "ScpiConnection.h"

#include <QString>


struct ChannelState
{
    QString waveform;
    double frequency = 0;
    double amplitude = 0;
    double offset = 0;
    double phase = 0;
};


class SDG2000X
{
public:

    bool connectTo(const QString& ip);

    QString identification();

    ChannelState getChannelState(int channel);

    bool setFrequency(int channel, double hz);

    bool setWaveform(int channel, const QString& waveform);

    bool setAmplitude(int channel, double volts);

    bool setOffset(int channel, double volts);

    bool output(int channel, bool enabled);

    bool clearErrors();

    bool getOutputState(int channel);

    QString getError();

    QString getConnectionError() const;

private:

    ScpiConnection scpi;

    QString channelPrefix(int channel);
};

