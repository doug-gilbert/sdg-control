/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

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

bool SDG2000X::setSdgWaveform(int channel, const QString& waveform)
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

bool SDG2000X::setSdgFrequency(int channel, double hz)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV FRQ,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(hz, 'f', 6));

    return scpi.command(cmd);
}

bool SDG2000X::setSdgAmplitude(int channel, const AmplitudeState &amp)
{
    if (!scpi.isConnected())
        return false;

    const ValueRepresentation vr = amp.valueRepresentation();

    if (vr.representation == "Vpp")
    {
        const QString cmd =
            QString("%1:BSWV AMP,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 3));

        return scpi.command(cmd);
    }

    if (vr.representation == "Vrms")
    {
        const QString cmd =
            QString("%1:BSWV AMPVRMS,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 6));

        return scpi.command(cmd);
    }

    if (vr.representation == "dBm")
    {
        const QString cmd =
            QString("%1:BSWV AMPDBM,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 6));

        return scpi.command(cmd);
    }

#if 0
    sdgDebug() << Q_FUNC_INFO
               << "Channel=" << channel
               << ">>> No valid amplitude representation"
               << "userRep=" << amp.userRepresentation
               << "vppValid=" << amp.v_ppValid
               << "vrmsValid=" << amp.v_rmsValid
               << "dBmValid=" << amp.dBmValid;
#endif

    return false;
}

bool SDG2000X::setSdgOffset(int channel, double volts)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV OFST,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'g', 4));

    return scpi.command(cmd);
}

bool SDG2000X::setSdgPhase(int channel, double degrees)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV PHSE,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(degrees, 'f', 1));

    return scpi.command(cmd);
}

bool SDG2000X::setSdgRampSymmetry(int channel, double percent)
{
    return scpi.command(
        QString("%1:BSWV SYM,%2")
            .arg(channelPrefix(channel))
            .arg(percent, 0, 'f', 1));
}

bool SDG2000X::setSdgPulseWidth(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV WIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgPulseRise(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV RISE,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgPulseFall(int channel, double seconds)
{
    return scpi.command(
        QString("%1:BSWV FALL,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgNoiseBandset(int channel, bool enabled)
{
    return scpi.command(
        QString("%1:BSWV BANDSTATE,%2")
            .arg(channelPrefix(channel))
            .arg(enabled ? "ON" : "OFF"));
}

bool SDG2000X::setSdgNoiseStdev(int channel, double volts)
{
    return scpi.command(
        QString("%1:BSWV STDEV,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setSdgNoiseMean(int channel, double volts)
{
    return scpi.command(
        QString("%1:BSWV MEAN,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setSdgNoiseBandwidth(int channel, double freq)
{
    return scpi.command(
        QString("%1:BSWV BANDWIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(freq, 0, 'g', 12));
}

bool SDG2000X::setSdgDcOffset(int channel, double value)
{
    return scpi.command(
        QString("%1:BSWV OFST,%2")
            .arg(channelPrefix(channel))
            .arg(value, 0, 'g', 4));
}

bool SDG2000X::setSdgDcPrecisionHigh(int channel, bool enabled)
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

bool SDG2000X::setSdgOutput(int channel, bool enabled)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:OUTP %2")
        .arg(channelPrefix(channel))
        .arg(enabled ? "ON" : "OFF");

    if (!scpi.command(cmd))
    {
        sdgDebug() << Q_FUNC_INFO << "scpi.command() returned false";
        return false;
    }

    auto state = getOutputState(channel);

    return state && state->enabled == enabled;
}

bool SDG2000X::setSdgOutputLoadPol(int channel, bool enabled, bool load50,
                                   bool polNormal)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:OUTP %2,LOAD,%3,PLRT,%4")
        .arg(channelPrefix(channel))
        .arg(enabled ? "ON" : "OFF")
        .arg(load50 ? "50" : "HZ")
        .arg(polNormal ? "NOR" : "INVT");

    if (!scpi.command(cmd))
    {
        sdgDebug() << Q_FUNC_INFO << "scpi.command() returned false";
        return false;
    }

    auto state = getOutputState(channel);

    return state && state->enabled == enabled;
}

bool SDG2000X::setSdgOutputBoth(bool enabled)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("OUT_BOTHCH %1")
        .arg(enabled ? "ON" : "OFF");

    if (!scpi.command(cmd))
    {
        sdgDebug() << Q_FUNC_INFO << "scpi.command() returned false";
        return false;
    }
    return true;
}

bool SDG2000X::invert(int channel, bool enabled)
{
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:INVT %2")
        .arg(channelPrefix(channel))
        .arg(enabled ? "ON" : "OFF");

    if (!scpi.command(cmd))
    {
        sdgDebug() << Q_FUNC_INFO << "scpi.command() returned false";
        return false;
    }
    return true;
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
            state.amplitude.setAmplitudeVpp(value.toDouble());
        }
        else if (key == "AMPVRMS")
        {
            value.remove("Vrms");
            state.amplitude.setAmplitudeVrms(value.toDouble());
        }
        else if (key == "AMPDBM")
        {
            value.remove("dBm");
            state.amplitude.setAmplitude_dBm(value.toDouble());
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
        else if (key == "DUTY")
        {
            state.duty = value.toDouble();
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

    const auto outputState = getOutputState(channel);

    if (outputState)
        state.output = *outputState;

    return state;
}

bool SDG2000X::clearErrors()
{
    if (!scpi.isConnected())
        return false;

    return scpi.command("*CLS");
}

std::optional<OutputState> SDG2000X::getOutputState(int channel)
{
    QString response =
        scpi.query(channelPrefix(channel) + ":OUTP?");

    sdgDebug() << "OUTP response:" << response;

    if (response.isEmpty())
        return std::nullopt;

    OutputState state;
    state.enabled = response.contains("OUTP ON");

    if (response.contains("LOAD,50"))
        state.load = OutputLoad::Ohm50;
    else if (response.contains("LOAD,HZ"))
        state.load = OutputLoad::HighZ;
    else
        return std::nullopt;

    return state;
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

    ok &= setSdgWaveform(channel, state.waveform);

    if (state.waveform == "RAMP")
    {
        ok &= setSdgFrequency(channel, state.frequency);
        ok &= setSdgAmplitude(channel, state.amplitude);
        ok &= setSdgOffset(channel, state.offset);
        ok &= setSdgPhase(channel, state.phase);
        ok &= setSdgRampSymmetry(channel, state.rampSymmetry);
    }
    else if (state.waveform == "PULSE")
    {
        ok &= setSdgFrequency(channel, state.frequency);
        ok &= setSdgAmplitude(channel, state.amplitude);
        ok &= setSdgOffset(channel, state.offset);
        ok &= setSdgPhase(channel, state.phase);

        ok &= setSdgPulseWidth(channel, state.pulseWidth);
        ok &= setSdgPulseRise(channel, state.pulseRise);
        ok &= setSdgPulseFall(channel, state.pulseFall);
    }
    else if (state.waveform == "NOISE")
    {
        ok &= setSdgNoiseBandset(channel, state.noiseBandset);
        ok &= setSdgNoiseStdev(channel, state.noiseStdev);
        ok &= setSdgNoiseMean(channel, state.noiseMean);

        if (state.noiseBandset)
            ok &= setSdgNoiseBandwidth(channel, state.noiseBandwidth);
    }
    else if (state.waveform == "DC")
    {
        ok &= setSdgDcOffset(channel, state.dcOffset);
        ok &= setSdgDcPrecisionHigh(channel, state.dcPrecisionHigh);
    }
    else
    {
        // SINE, SQUARE, ARB, etc.
        ok &= setSdgFrequency(channel, state.frequency);
        ok &= setSdgAmplitude(channel, state.amplitude);
        ok &= setSdgOffset(channel, state.offset);
        ok &= setSdgPhase(channel, state.phase);
        if (state.waveform == "SQUARE")
            ok &= setSdgDuty(channel, state.duty);
    }

    // Wait up to 5 seconds, could be connection lost
    ok &= waitForOperationComplete(5000);
    ok &= setSdgOutput(channel, state.output.enabled);

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

bool SDG2000X::setSdgDuty(int channel, double percent)
{
    if (percent < 0.0 || percent > 100.0) {
        sdgDebug() << Q_FUNC_INFO << "bad percentage:" << percent;
        return false;
    }
    if (!scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV DUTY,%2")
        .arg(channelPrefix(channel))
        .arg(percent, 0, 'f', 3);

    return scpi.command(cmd);
}
