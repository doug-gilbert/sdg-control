#pragma once

#include "ScpiConnection.h"
#include "Instrument.h"
#include "ChannelState.h"

#include <QString>


class SDG2000X : public QObject, public Instrument
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

    bool setFrequency(int channel, double hz);

    bool setWaveform(int channel, const QString& waveform);

    bool setAmplitude(int channel, double volts);

    bool setOffset(int channel, double volts);

    bool setPhase(int channel, double degrees);

    bool setSymmetry(int channel, double percent);

    bool output(int channel, bool enabled);

    bool clearErrors();

    bool getOutputState(int channel);

    bool waitForOperationComplete();

    QString getError();

signals:
    void disconnected();

private:

    ScpiConnection scpi;

    QString channelPrefix(int channel);
};
