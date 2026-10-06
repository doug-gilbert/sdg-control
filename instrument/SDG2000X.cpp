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
#include "Utility.h"    /* in common sub-directory */
#include "debug.h"      /* in common sub-directory */


SDG2000X::SDG2000X(QObject *parent)
    : Instrument(parent)
{
    connect(&m_scpi,
            &ScpiConnection::disconnected,
            this,
            &SDG2000X::disconnected);
}

SDG2000X::~SDG2000X()
{
    DEBUG_FUNC;
}

// Yes the SDG2000X has a front panel, the simulator doesn't
bool SDG2000X::hasFrontPanel() const
{
    return true;
}

bool SDG2000X::connectTo(const QString& ip)
{
    return m_scpi.connectTo(ip);
}

void SDG2000X::disconnect()
{
    m_scpi.disconnect();
}

bool SDG2000X::isConnected() const
{
    return m_scpi.isConnected();
}

QString SDG2000X::identification()
{
    return m_scpi.query("*IDN?");
}

QString SDG2000X::channelPrefix(int channel)
{
    return QString("C%1").arg(channel);
}

bool SDG2000X::setSdgWaveform(int channel, const QString& waveform)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV WVTP,%2")
        .arg(channelPrefix(channel))
        .arg(waveform);

    sdgDebug() << "Waveform:" << cmd;

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgFrequency(int channel, double hz)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV FRQ,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(hz, 'f', 6));

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgAmplitude(int channel, const AmplitudeState &amp)
{
    if (!m_scpi.isConnected())
        return false;

    const ValueRepresentation vr = amp.valueRepresentation();

    if (vr.representation == "Vpp")
    {
        const QString cmd =
            QString("%1:BSWV AMP,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 3));

        return m_scpi.command(cmd);
    }

    if (vr.representation == "Vrms")
    {
        const QString cmd =
            QString("%1:BSWV AMPVRMS,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 6));

        return m_scpi.command(cmd);
    }

    if (vr.representation == "dBm")
    {
        const QString cmd =
            QString("%1:BSWV AMPDBM,%2")
                .arg(channelPrefix(channel))
                .arg(QString::number(vr.value, 'f', 6));

        return m_scpi.command(cmd);
    }

#if 0
    DEBUG_FUNC << "Channel=" << channel
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
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV OFST,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'g', 4));

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgVHigh(int channel, double volts)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV HLEV,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'g', 4));

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgVLow(int channel, double volts)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV LLEV,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(volts, 'g', 4));

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgPhase(int channel, double degrees)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV PHSE,%2")
        .arg(channelPrefix(channel))
        .arg(QString::number(degrees, 'f', 1));

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgRampSymmetry(int channel, double percent)
{
    return m_scpi.command(
        QString("%1:BSWV SYM,%2")
            .arg(channelPrefix(channel))
            .arg(percent, 0, 'f', 1));
}

bool SDG2000X::setSdgPulseWidth(int channel, double seconds)
{
    return m_scpi.command(
        QString("%1:BSWV WIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgPulseRise(int channel, double seconds)
{
    return m_scpi.command(
        QString("%1:BSWV RISE,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgPulseFall(int channel, double seconds)
{
    return m_scpi.command(
        QString("%1:BSWV FALL,%2")
            .arg(channelPrefix(channel))
            .arg(QString::number(seconds, 'g', 12)));
}

bool SDG2000X::setSdgNoiseBandset(int channel, bool enabled)
{
    return m_scpi.command(
        QString("%1:BSWV BANDSTATE,%2")
            .arg(channelPrefix(channel))
            .arg(enabled ? "ON" : "OFF"));
}

bool SDG2000X::setSdgNoiseStdev(int channel, double volts)
{
    return m_scpi.command(
        QString("%1:BSWV STDEV,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setSdgNoiseMean(int channel, double volts)
{
    return m_scpi.command(
        QString("%1:BSWV MEAN,%2")
            .arg(channelPrefix(channel))
            .arg(volts, 0, 'g', 12));
}

bool SDG2000X::setSdgNoiseBandwidth(int channel, double freq)
{
    return m_scpi.command(
        QString("%1:BSWV BANDWIDTH,%2")
            .arg(channelPrefix(channel))
            .arg(freq, 0, 'g', 12));
}

bool SDG2000X::setSdgDcOffset(int channel, double value)
{
    return m_scpi.command(
        QString("%1:BSWV OFST,%2")
            .arg(channelPrefix(channel))
            .arg(value, 0, 'g', 4));
}

bool SDG2000X::setSdgDcPrecisionHigh(int channel, bool enabled)
{
#if 0           // not defined in Prog. manual, not returned by SDG ??
    return m_scpi.command(
        QString("%1:BSWV PRECISION,%2")
            .arg(channelPrefix(channel))
            .arg(enabled ? "HIGH" : "LOW"));
#else
    Q_UNUSED(channel);
    Q_UNUSED(enabled);
    return true;
#endif
}

bool SDG2000X::setSdgExternalOutput(int channel, bool externalOutput)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:OUTP %2")
        .arg(channelPrefix(channel))
        .arg(externalOutput ? "ON" : "OFF");

    if (!m_scpi.command(cmd))
    {
        DEBUG_FUNC << "CH" << channel << "  m_scpi.command() returned false";
        return false;
    }

    auto state = getOutputState(channel);

    return state && state->externalOutput == externalOutput;
}

bool SDG2000X::setSdgOutputLoadPol(int channel, const OutputState &oState)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:OUTP %2,LOAD,%3,PLRT,%4")
        .arg(channelPrefix(channel))
        .arg(oState.externalOutput ? "ON" : "OFF")
        .arg(oState.outputLoad == OutputLoad::Ohms50 ? "50" : "HZ")
        .arg(oState.polarity == Polarity::Normal ? "NOR" : "INVT");

    if (!m_scpi.command(cmd))
    {
        DEBUG_FUNC << "CH" << channel << ": m_scpi.command() returned false";
        return false;
    }

    auto state = getOutputState(channel);

    if (state)
    {
        bool ok = true;

        if (state->externalOutput != oState.externalOutput)
        {
            sdgDebug() << "CH" << channel << ": setting main Output failed";
            ok = false;
        }
        if (state->outputLoad != oState.outputLoad)
        {
            DEBUG_FUNC << "CH" << channel << ": setting OutputLoad failed";
            ok = false;
        }
        if (state->polarity != oState.polarity)
        {
            DEBUG_FUNC << "CH" << channel << "setting Polarity failed";
            ok = false;
        }
        return ok;
    }
    else
        return false;
}

bool SDG2000X::setSdgOutputBoth(bool enabled)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("OUT_BOTHCH %1")
        .arg(enabled ? "ON" : "OFF");

    if (!m_scpi.command(cmd))
    {
        DEBUG_FUNC << "m_scpi.command() returned false";
        return false;
    }
    return true;
}

bool SDG2000X::setInvert(int channel, bool enable)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:INVT %2")
        .arg(channelPrefix(channel))
        .arg(enable ? "ON" : "OFF");

    if (!m_scpi.command(cmd))
    {
        DEBUG_FUNC << "CH" << channel
                       << ": m_scpi.command() returned false";
        return false;
    }
    return true;
}

bool SDG2000X::setSdgClockSource(ClockSource cs)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd = QString("ROSC %1")
        .arg((cs == ClockSource::Internal) ? "INTERNAL" : "EXTERNAL");

    sdgDebug() << "Clock source:" << cmd;

    return m_scpi.command(cmd);
}

bool SDG2000X::setSdgOverVoltageProtection(OverVoltageProtection ovp)
{
    if (!m_scpi.isConnected())
        return false;

    QString cmd = QString("VOLTPRT %1")
        .arg((ovp == OverVoltageProtection::Off) ? "OFF" : "ON");

    sdgDebug() << "Over Voltage Protection:" << cmd;

    return m_scpi.command(cmd);
}

ChannelState SDG2000X::getChannelState(int channel)
{
    ChannelState state;
    QString wvtp;
    QString response =
        m_scpi.query(channelPrefix(channel) + ":BSWV?");

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
        else if (key == "HLEV")
        {
            value.remove("V");
            state.vHigh = value.toDouble();
        }
        else if (key == "LLEV")
        {
            value.remove("V");
            state.vLow = value.toDouble();
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
        else if (key == "MAX_OUTPUT_AMP")
        {
            value.remove("V");
            state.maxOutputAmplitude = value.toDouble();
        }
    }

    const auto outputState = getOutputState(channel);

    if (outputState)
        state.output = *outputState;

    return state;
}

GeneralState SDG2000X::getGeneralState()
{
    GeneralState state;

    QString response = m_scpi.query("VOLTPRT?");

    DEBUG_FUNC << "VOLTPRT? response:" << response;
    if (response.contains("OFF"))
        state.overVoltageProtection = OverVoltageProtection::Off;
    else if (response.contains("ON"))
        state.overVoltageProtection = OverVoltageProtection::On;

    response = m_scpi.query("ROSC?");
    sdgDebug() << "ROSC? response:" << response;
    if (response.contains("INTERNAL"))
        state.clockSource = ClockSource::Internal;
    else if (response.contains("EXTERNAL"))
        state.clockSource = ClockSource::External;

    return state;
}

#if 0
ChannelState SDG2000X::getSweepState(int channel)
{
    ChannelState state;
    QString wvtp;
    QString response = m_scpi.query(channelPrefix(channel) + ":SWWV?");
    /* ARWV for Arb waveforms, MDWV for modulated, BTWV for Burst */

    sdgDebug() << "SWWV raw response:" << response;

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
        /* lots more to decode here */
    }

    const auto outputState = getOutputState(channel);

    if (outputState)
        state.output = *outputState;

    return state;
}
#endif

bool SDG2000X::clearErrors()
{
    if (!m_scpi.isConnected())
        return false;

    return m_scpi.command("*CLS");
}

std::optional<OutputState> SDG2000X::getOutputState(int channel)
{
    QString response =
        m_scpi.query(channelPrefix(channel) + ":OUTP?");

    sdgDebug() << "OUTP response:" << response;

    if (response.isEmpty())
        return std::nullopt;

    OutputState state;
    state.externalOutput = response.contains("OUTP ON");

    if (response.contains("PLRT,NOR"))
        state.polarity = Polarity::Normal;
    else if (response.contains("PLRT,INVT"))
        state.polarity = Polarity::Inverted;

    if (response.contains("LOAD,50"))
        state.outputLoad = OutputLoad::Ohms50;
    else if (response.contains("LOAD,HZ"))
        state.outputLoad = OutputLoad::HiZ;

    state.powerOnState = response.contains("POWERON_STATE,ON");

qsdgDebug() << "ExtrenalOutput=" << state.externalOutput
            << "OutputLoad=" << Utility::outputLoadToString(state.outputLoad)
            << "Polarity=" << Utility::polarityToString(state.polarity)
            << "PowerOn_State=" << state.powerOnState;
    return state;
}

QString SDG2000X::getError()
{
    return m_scpi.query("SYST:ERR?");
}

QString SDG2000X::getConnectionError() const
{
    return m_scpi.errorString();
}

bool SDG2000X::waitForOperationComplete(int timeout_ms)
{
    return m_scpi.waitForOperationComplete(timeout_ms);
}

bool SDG2000X::applyVoltageHelper(int channel, const ChannelState& state,
                                   const ChannelDirtyState& dirty)
{
    bool ok = true;

    if (dirty.m_amplitude || dirty.m_offset)
    {
        if (dirty.m_amplitude)
            ok &= setSdgAmplitude(channel, state.amplitude);
        if (dirty.m_offset)
            ok &= setSdgOffset(channel, state.offset);
    }
    else
    {         // give Amplitude/Offset precedence over Vhigh/Vlow
        if (dirty.m_vHigh)
            ok &= setSdgVHigh(channel, state.vLow);
        if (dirty.m_vLow)
            ok &= setSdgVLow(channel, state.vHigh);
    }
    return ok;
}

bool SDG2000X::applyChannelState(int channel, const ChannelState& state,
                                 const ChannelDirtyState& dirty)
{
    bool ok = true;
    const uint32_t scpiStart = m_scpi.numScpiSent();

    DEBUG_FUNC << "SCPI count at entry =" << scpiStart << ", waveform ="
               << state.waveform;
    if (dirty.m_waveform)
        ok &= setSdgWaveform(channel, state.waveform);

    if (state.waveform == "RAMP")
    {
        if (dirty.m_frequency)
            ok &= setSdgFrequency(channel, state.frequency);
        ok &= applyVoltageHelper(channel, state, dirty);
        if (dirty.m_phase)
            ok &= setSdgPhase(channel, state.phase);
        if (dirty.m_rampSymmetry)
            ok &= setSdgRampSymmetry(channel, state.rampSymmetry);
    }
    else if (state.waveform == "PULSE")
    {
        if (dirty.m_frequency)
            ok &= setSdgFrequency(channel, state.frequency);
        ok &= applyVoltageHelper(channel, state, dirty);
        if (dirty.m_phase)
            ok &= setSdgPhase(channel, state.phase);
        if (dirty.m_pulseWidth)
            ok &= setSdgPulseWidth(channel, state.pulseWidth);
        if (dirty.m_pulseRise)
            ok &= setSdgPulseRise(channel, state.pulseRise);
        if (dirty.m_pulseFall)
            ok &= setSdgPulseFall(channel, state.pulseFall);
    }
    else if (state.waveform == "NOISE")
    {
        if (dirty.m_noiseBandset)
            ok &= setSdgNoiseBandset(channel, state.noiseBandset);
        if (dirty.m_noiseStdev)
            ok &= setSdgNoiseStdev(channel, state.noiseStdev);
        if (dirty.m_noiseMean)
            ok &= setSdgNoiseMean(channel, state.noiseMean);

        if (state.noiseBandwidth)
            ok &= setSdgNoiseBandwidth(channel, state.noiseBandwidth);
    }
    else if (state.waveform == "DC")
    {
        if (dirty.m_dcOffset)
            ok &= setSdgDcOffset(channel, state.dcOffset);
        if (dirty.m_dcPrecisionHigh)
            ok &= setSdgDcPrecisionHigh(channel, state.dcPrecisionHigh);
    }
    else
    {
        // SINE, SQUARE, ARB, etc.
        if (dirty.m_frequency)
            ok &= setSdgFrequency(channel, state.frequency);
        ok &= applyVoltageHelper(channel, state, dirty);
        if (dirty.m_phase)
            ok &= setSdgPhase(channel, state.phase);
        if (dirty.m_duty && state.waveform == "SQUARE")
            ok &= setSdgDuty(channel, state.duty);
    }

    if (dirty.m_externalOutput || dirty.m_polarity || dirty.m_outputLoad) {
        if (m_scpi.numScpiSent() != scpiStart)
            ok &= waitForOperationComplete(5000);
        // Following command was observed to ignore requests when SDG busy
        ok &= setSdgOutputLoadPol(channel, state.output);
    }

    const uint32_t scpiEnd = m_scpi.numScpiSent();

    DEBUG_FUNC << "SCPI count at exit =" << scpiEnd;

    if (scpiEnd != scpiStart) {
        DEBUG_FUNC << "Commands sent; waiting for OPC";
        // Wait up to 5 seconds, could be connection lost
        return ok & m_scpi.waitForOperationComplete(5000);
    }

    DEBUG_FUNC << "No SCPI commands sent; skipping OPC";
    return true;
}

bool SDG2000X::applyGeneralState(const GeneralState& g_state,
                                 const GeneralDirtyState& dirty)
{
    bool ok = true;

    if (dirty.m_clockSource)
        setSdgClockSource(g_state.clockSource);

    if (dirty.m_overVoltageProtection)
        setSdgOverVoltageProtection(g_state.overVoltageProtection);

    return ok;
}

QByteArray SDG2000X::getFrontPanelImage()
{
    if (!m_scpi.isConnected())
        return {};

    return m_scpi.queryBinary("SCDP");
}

bool SDG2000X::toggleChannelFocus()
{
    if (!m_scpi.isConnected())
        return false;

    waitForOperationComplete(5000);
    return m_scpi.command("VKEY VALUE,KB_CHANNEL,STATE,1");
}

bool SDG2000X::reset()
{
    if (!m_scpi.isConnected())
        return false;

    sdgDebug() << "about to issue RESET";

    if (!m_scpi.command("*RST"))
        return false;

    return waitForOperationComplete(5000);
}

bool SDG2000X::setSdgDuty(int channel, double percent)
{
    if (percent < 0.0 || percent > 100.0) {
        DEBUG_FUNC << "bad percentage:" << percent;
        return false;
    }
    if (!m_scpi.isConnected())
        return false;

    QString cmd =
        QString("%1:BSWV DUTY,%2")
        .arg(channelPrefix(channel))
        .arg(percent, 0, 'f', 3);

    return m_scpi.command(cmd);
}
