#pragma once

#include <QtTypes>

#include "ChannelState.h"

class QString;

class Instrument
{
public:
    virtual ~Instrument() = default;

    virtual bool connectToHost(const QString &host,
                               quint16 port) = 0;

    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual bool identify(QString &idn) = 0;

    virtual bool applyChannelState(int channel,
                                   const ChannelState &state) = 0;
};
