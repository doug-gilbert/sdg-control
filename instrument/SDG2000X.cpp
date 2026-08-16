
#include <QRegularExpression>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "SDG2000X.h"
#include "debug.h"      /* in common sub-directory */


SDG2000X::SDG2000X(QObject *parent)
    : Instrument(parent)
{
    connect(&scpi,
            &ScpiConnection::disconnected,
            this,
            &SDG2000X::disconnected);
}

SDG2000X::~SDG2000X()
{
    sdgDebug() << "SDG2000X destructor";
}

// Yes the SDG2000X has a front panel, the simulator doesnt
bool SDG2000X::hasFrontPanel() const
{
    return true;
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

bool SDG2000X::setAmplitudeVrms(int channel, double volts)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV AMPVRMS,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'f', 6));

    return scpi.command(cmd);
}

bool SDG2000X::setAmplitudedBm(int channel, double dbm)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV AMPDBM,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(dbm, 'f', 6));

    return scpi.command(cmd);
}

bool SDG2000X::setUserAmplitude(
    int channel, const SdgAmplitude &amplitude)
{
    switch (amplitude.userRepresentation())
    {
    case SdgAmplitude::Representation::Vpp:
        return setAmplitude(channel, amplitude.userValue());

    case SdgAmplitude::Representation::Vrms:
        return setAmplitudeVrms(channel, amplitude.userValue());

    case SdgAmplitude::Representation::dBm:
        return setAmplitudedBm(channel, amplitude.userValue());
    }

    return false;
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

bool SDG2000X::setRampSymmetry(int channel, double percent)
{
    return scpi.command(
        QString("%1:BSWV SYM,%2")
            .arg(channelPrefix(channel))
            .arg(percent, 0, 'f', 1));
}

bool SDG2000X::setPulseWidth(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV WIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setPulseRise(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV RISE,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setPulseFall(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV FALL,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setNoiseBandset(int channel, bool enabled)
{
    return scpi.command(
        QString("%1:BSWV BANDSTATE,%2")
            .arg(channelPrefix(channel))
            .arg(enabled ? "ON" : "OFF"));
}

bool SDG2000X::setNoiseStdev(int channel, double volts)
{
    return scpi.command(
        QString("%1:BSWV STDEV,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setNoiseMean(int channel, double volts)
{
    return scpi.command(
        QString("%1:BSWV MEAN,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setNoiseBandwidth(int channel, double freq)
{
    return scpi.command(
        QString("%1:BSWV BANDWIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(freq, 0, 'g', 12));
}

bool SDG2000X::setDcOffset(int channel, double value)
{
    return scpi.command(
        QString("%1:BSWV OFST,%2")
            .arg(channelPrefix(channel))
            .arg(value, 0, 'g', 12));
}

bool SDG2000X::setDcPrecisionHigh(int channel, bool enabled)
{
#if 0           // not defined in Prog. manual, not returned by SDG ??
    return scpi.command(
        QString("%1:BSWV PRECISION,%2")
            .arg(channelPrefix(channel))
            .arg(enabled ? "HIGH" : "LOW"));
#else
    Q_UNUSED(channel);
    Q_UNUSED(enabled);
    return true;
#endif
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
    {
        sdgDebug() << __func__ << "scpi.command() returned false";
        return false;
    }

    return getOutputState(channel) == enabled;
}

ChannelState SDG2000X::getChannelState(int channel)
{
    ChannelState state;
    QString wvtp;
    QString response =
        scpi.query(channelPrefix(channel) + ":BSWV?");

    sdgDebug() << "BSWV raw response:" << response;

    if (response == "WRITE ERROR" ||
        response == "READ TIMEOUT")
    {
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
            wvtp = value;
        }
        else if (key == "FRQ")
        {
            value.remove("HZ");
            state.frequency = value.toDouble();
        }
        else if (key == "AMP")
        {
            value.remove("V");
            state.amplitude.setInstrumentVpp(value.toDouble());
        }
        else if (key == "AMPVRMS")
        {
            value.remove("Vrms");
            state.amplitude.setInstrumentVrms(value.toDouble());
        }
        else if (key == "AMPDBM")
        {
            value.remove("dBm");
            state.amplitude.setInstrumentdBm(value.toDouble());
        }
        else if (key == "OFST")
        {
            value.remove("V");
            if (wvtp == "DC")
                state.dcOffset = value.toDouble();
            else
                state.offset = value.toDouble();
        }
        else if (key == "PHSE")
        {
            state.phase = value.toDouble();
        }
        else if (key == "SYM")
        {
            state.rampSymmetry = value.toDouble();
        }
        else if (key == "WIDTH")
        {
            value.remove("S");
            state.pulseWidth = value.toDouble();
        }
        else if (key == "RISE")
        {
            value.remove("S");
            state.pulseRise = value.toDouble();
        }
        else if (key == "FALL")
        {
            value.remove("S");
            state.pulseFall = value.toDouble();
        }
        else if (key == "BANDSTATE")
        {
            state.noiseBandset = (value == "ON");
        }
        else if (key == "STDEV")
        {
            value.remove("V");
            state.noiseStdev = value.toDouble();
        }
        else if (key == "MEAN")
        {
            value.remove("V");
            state.noiseMean = value.toDouble();
        }
        else if (key == "BANDWIDTH")
        {
            value.remove("HZ");
            state.noiseBandwidth = value.toDouble();
        }
    }

    state.output = getOutputState(channel);

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

bool SDG2000X::waitForOperationComplete(int timeout_ms)
{
    return scpi.waitForOperationComplete(timeout_ms);
}

bool SDG2000X::applyChannelState(int channel, const ChannelState& state)
{
    bool ok = true;

    ok &= setWaveform(channel, state.waveform);

    if (state.waveform == "RAMP")
    {
        ok &= setFrequency(channel, state.frequency);
        ok &= setUserAmplitude(channel, state.amplitude);
        ok &= setOffset(channel, state.offset);
        ok &= setPhase(channel, state.phase);
        ok &= setRampSymmetry(channel, state.rampSymmetry);
    }
    else if (state.waveform == "PULSE")
    {
        ok &= setFrequency(channel, state.frequency);
        ok &= setUserAmplitude(channel, state.amplitude);
        ok &= setOffset(channel, state.offset);
        ok &= setPhase(channel, state.phase);

        ok &= setPulseWidth(channel, state.pulseWidth);
        ok &= setPulseRise(channel, state.pulseRise);
        ok &= setPulseFall(channel, state.pulseFall);
    }
    else if (state.waveform == "NOISE")
    {
        ok &= setNoiseBandset(channel, state.noiseBandset);
        ok &= setNoiseStdev(channel, state.noiseStdev);
        ok &= setNoiseMean(channel, state.noiseMean);

        if (state.noiseBandset)
            ok &= setNoiseBandwidth(channel, state.noiseBandwidth);
    }
    else if (state.waveform == "DC")
    {
        ok &= setDcOffset(channel, state.dcOffset);
        ok &= setDcPrecisionHigh(channel, state.dcPrecisionHigh);
    }
    else
    {
        // SINE, SQUARE, ARB, etc.
        ok &= setFrequency(channel, state.frequency);
        ok &= setUserAmplitude(channel, state.amplitude);
        ok &= setOffset(channel, state.offset);
        ok &= setPhase(channel, state.phase);
    }

    // Wait up to 5 seconds, could be connection lost
    ok &= waitForOperationComplete(5000);
    ok &= output(channel, state.output);

    return ok;
}

QByteArray SDG2000X::getFrontPanelImage()
{
    if (!scpi.isConnected())
        return {};

    return scpi.queryBinary("SCDP");
}

bool SDG2000X::toggleChannelFocus()
{
    if (!scpi.isConnected())
        return false;

    return scpi.command("VKEY VALUE,KB_CHANNEL,STATE,1");
}

bool SDG2000X::reset()
{
    if (!scpi.isConnected())
        return false;

    sdgDebug() << "about to issue RESET";

    if (!scpi.command("*RST"))
        return false;

    return waitForOperationComplete(5000);
}
