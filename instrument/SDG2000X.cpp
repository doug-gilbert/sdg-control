
#include <QRegularExpression>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "SDG2000X.h"
#include "debug.h"      /* in common sub-directory */


SDG2000X::SDG2000X(QObject *parent)
    : QObject(parent)
{
    connect(&scpi,
            &ScpiConnection::disconnected,
            this,
            &SDG2000X::disconnected);
}

bool SDG2000X::connectTo(const QString& ip)
{
    return scpi.connectTo(ip);
}

void SDG2000X::disconnect()
{
    scpi.disconnect();
}

bool SDG2000X::isConnected() const
{
    return scpi.isConnected();
}

QString SDG2000X::identification()
{
    return scpi.query("*IDN?");
}

QString SDG2000X::channelPrefix(int channel)
{
    return QString("C%1").arg(channel);
}

bool SDG2000X::setFrequency(int channel, double hz)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV FRQ,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(hz, 'f', 6));

    return scpi.command(cmd);
}

bool SDG2000X::setWaveform(int channel, const QString& waveform)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV WVTP,%2")
        .arg(channelPrefix(channel))
        .arg(waveform);

    sdgDebug() << "Waveform:" << cmd;

    return scpi.command(cmd);
}

bool SDG2000X::setAmplitude(int channel, double volts)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV AMP,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'f', 3));

    return scpi.command(cmd);
}

bool SDG2000X::setOffset(int channel, double volts)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV OFST,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'f', 3));

    return scpi.command(cmd);
}

bool SDG2000X::setSymmetry(int channel, double percent)
{
    return scpi.command(
        QString("%1:BSWV SYM,%2")
            .arg(channelPrefix(channel))
            .arg(percent, 0, 'f', 1));
}

bool SDG2000X::output(int channel, bool enabled)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:OUTP %2")
        .arg(channelPrefix(channel))
        .arg(enabled ? "ON" : "OFF");

    if (!scpi.command(cmd))
        return false;

    return getOutputState(channel) == enabled;
}

ChannelState SDG2000X::getChannelState(int channel)
{
    ChannelState state;

    QString response =
        scpi.query(channelPrefix(channel) + ":BSWV?");

    sdgDebug() << "BSWV raw response:" << response;

    if (response == "WRITE ERROR") {
        state.waveform = response;
        return state;
    } else if (response == "READ TIMEOUT") {
        state.waveform = response;
        return state;
    }

    QStringList fields = response.split(',');

    for (int i = 0; i + 1 < fields.size(); i += 2)
    {
        QString key = fields[i].trimmed();
        QString value = fields[i + 1].trimmed();

        if (key.contains("WVTP"))
            key = "WVTP";

        if (key == "WVTP")
        {
            state.waveform = value;
        }
        else if (key == "FRQ")
        {
            value.remove("HZ");
            state.frequency = value.toDouble();
        }
        else if (key == "AMP")
        {
            value.remove("V");
            state.amplitude = value.toDouble();
        }
        else if (key == "OFST")
        {
            value.remove("V");
            state.offset = value.toDouble();
        }
        else if (key == "PHSE")
        {
            state.phase = value.toDouble();
        }
        else if (key == "SYM")
        {
            state.symmetry = value.toDouble();
        }
    }

    return state;
}

bool SDG2000X::clearErrors()
{
    if (!scpi.isConnected())
        return false;

    return scpi.command("*CLS");
}

bool SDG2000X::getOutputState(int channel)
{
    QString response =
        scpi.query(channelPrefix(channel) + ":OUTP?");

    sdgDebug() << "OUTP response:" << response;

    return response.contains("OUTP ON");
}

QString SDG2000X::getError()
{
    return scpi.query("SYST:ERR?");
}

QString SDG2000X::getConnectionError() const
{
    return scpi.errorString();
}

bool SDG2000X::setPhase(int channel, double degrees)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV PHSE,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(degrees, 'f', 1));

    return scpi.command(cmd);
}
