#pragma once

#include <QtTypes>

#include "ChannelState.h"

class QString;

class Instrument
{
public:
    virtual ~Instrument() = default;

    virtual bool connectTo(const QString &host) = 0;

    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual QString identification() = 0;

    virtual bool applyChannelState(int channel,
                                   const ChannelState &state) = 0;

    virtual ChannelState getChannelState(int channel) = 0;

    virtual QString getConnectionError() const = 0;
};

