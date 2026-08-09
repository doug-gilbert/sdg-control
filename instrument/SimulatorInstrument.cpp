

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "SimulatorInstrument.h"


SimulatorInstrument::SimulatorInstrument(QObject *parent)
    : Instrument(parent)
{
}

bool SimulatorInstrument::connectTo(const QString &host)
{
    Q_UNUSED(host);

    connected = true;
    return true;
}

void SimulatorInstrument::disconnect()
{
    if (!connected)
        return;

    connected = false;
    emit disconnected();
}

bool SimulatorInstrument::isConnected() const
{
    return connected;
}

QString SimulatorInstrument::identification()
{
    return "SDG-Control Simulator";
}

QString SimulatorInstrument::getConnectionError() const
{
    return {};
}

ChannelState SimulatorInstrument::getChannelState(int channel)
{
    if (!connected || channel < 1 || channel > 2)
        return {};

    return channelState[channel - 1];
}

bool SimulatorInstrument::applyChannelState(
    int channel,
    const ChannelState &state)
{
    if (!connected || channel < 1 || channel > 2)
        return false;

    channelState[channel - 1] = state;
    return true;
}
