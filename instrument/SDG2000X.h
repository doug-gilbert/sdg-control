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


class SDG2000X : public QObject
{
    Q_OBJECT

public:

    explicit SDG2000X(QObject *parent = nullptr);

    bool connectTo(const QString& ip);

    void disconnect();

    bool isConnected() const;

    QString identification();

    ChannelState getChannelState(int channel);

    bool setFrequency(int channel, double hz);

    bool setWaveform(int channel, const QString& waveform);

    bool setAmplitude(int channel, double volts);

    bool setOffset(int channel, double volts);

    bool setPhase(int channel, double degrees);

    bool output(int channel, bool enabled);

    bool clearErrors();

    bool getOutputState(int channel);

    QString getError();

    QString getConnectionError() const;

signals:
    void disconnected();

private:

    ScpiConnection scpi;

    QString channelPrefix(int channel);
};
