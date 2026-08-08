#pragma once

#include "Instrument.h"

#include <array>

class SimulatorInstrument : public Instrument
{
public:
    explicit SimulatorInstrument(QObject *parent = nullptr);

    bool connectTo(const QString &host) override;
    void disconnect() override;
    bool isConnected() const override;

    QString identification() override;

    QString getConnectionError() const override;

    ChannelState getChannelState(int channel) override;

    bool applyChannelState(int channel,
                           const ChannelState &state) override;

private:
    std::array<ChannelState, 2> channelState;
    bool connected = false;
};
