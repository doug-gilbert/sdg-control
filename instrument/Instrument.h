#pragma once

#include <QObject>

#include "ChannelState.h"

class QString;

class Instrument : public QObject
{
    Q_OBJECT

public:
    explicit Instrument(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    ~Instrument() override = default;

    virtual bool connectTo(const QString &host) = 0;

    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual QString identification() = 0;

    virtual QString getConnectionError() const = 0;

    virtual ChannelState getChannelState(int channel) = 0;

    virtual bool applyChannelState(int channel,
                                   const ChannelState &state) = 0;

signals:
    void disconnected();
};

