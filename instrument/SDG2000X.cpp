#include "SDG2000X.h"

#include <QRegularExpression>
#ifdef SDG_DEBUG
#include <QDebug>
#endif


bool SDG2000X::connectTo(const QString& ip)
{
    return scpi.connectTo(ip);
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
    QString cmd =
        QString("%1:BSWV FRQ,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(hz, 'f', 6));

    return scpi.command(cmd);
}


bool SDG2000X::setWaveform(int channel, const QString& waveform)
{
    QString cmd =
        QString("%1:BSWV WVTP,%2")
        .arg(channelPrefix(channel))
        .arg(waveform);

    return scpi.command(cmd);
}


bool SDG2000X::setAmplitude(int channel, double volts)
{
    QString cmd =
        QString("%1:BSWV AMP,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'f', 3));

    return scpi.command(cmd);
}


bool SDG2000X::setOffset(int channel, double volts)
{
    QString cmd =
        QString("%1:BSWV OFST,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'f', 3));

    return scpi.command(cmd);
}


bool SDG2000X::output(int channel, bool enabled)
{
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

#ifdef SDG_DEBUG
qDebug() << "BSWV raw response:" << response;
#endif

    QStringList fields =
        response.split(',');

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
    }

    return state;
}

bool SDG2000X::clearErrors()
{
    return scpi.command("*CLS");
}

bool SDG2000X::getOutputState(int channel)
{
    QString response =
        scpi.query(channelPrefix(channel) + ":OUTP?");

#ifdef SDG_DEBUG
    qDebug() << "OUTP response:" << response;
#endif

    return response.contains("OUTP ON");
}

QString SDG2000X::getError()
{
    return scpi.query("SYST:ERR?");
}
