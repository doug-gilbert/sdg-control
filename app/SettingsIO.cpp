/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDateTime>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "SettingsIO.h"
#include "Utility.h"
#include "debug.h"


namespace
{
    constexpr int FormatVersion = 1;
    constexpr auto UtilityNameKey = "utility_name";
    constexpr auto utilityName = "sdg-control";
    constexpr auto UtilityVersionKey = "utility_version";
#ifdef SDG_CONTROL_VERSION
    #define MY_STRINGIFY(x) #x
    constexpr auto utilityVersion = MY_STRINGIFY(SDG_CONTROL_VERSION);
#else
    constexpr auto utilityVersion = "not available";
#endif
    constexpr auto ControlledInstrumentKey = "controlled_instrument";
    constexpr auto controlled_instrument =
          "SDG2000X series Function/Arbitrary Waveform Generator";

    /* JSON key names should only contain, a-z, 0-9 and _  */
    constexpr auto FormatVersionKey = "format_version";
    constexpr auto DateTimeKey      = "create_date_time";
    constexpr auto Channel1Key      = "channel1";
    constexpr auto Channel2Key      = "channel2";
    constexpr auto GeneralKey       = "general";
    constexpr auto ClockSourceKey   = "clock_source";
    constexpr auto OverVoltageProtectionKey   = "over_voltage_protection";
    constexpr auto SdgModeKey       = "mode";

    constexpr auto WaveformKey      = "waveform";
    constexpr auto BasicSettingsKey = "basic_settings";
    constexpr auto WaveformQualifierKey = "waveform_qualifier";
    constexpr auto FrequencyKey     = "frequency";
    constexpr auto AmplitudeKey     = "amplitude";
    constexpr auto AmplitudeRepKey  = "amplitude_representation";
    constexpr auto AmplitudeUserRepKey =
                                "amplitude_user_representation";
    constexpr auto OffsetKey        = "offset";
    constexpr auto VHighKey         = "volt_high";
    constexpr auto VLowKey          = "volt_low";
    constexpr auto PhaseKey         = "phase";
    constexpr auto DutyKey          = "duty";
    constexpr auto SymmetryKey      = "ramp_symmetry";
    constexpr auto PulseWidthKey    = "pulse_width";
    constexpr auto PulseRiseKey     = "pulse_rise";
    constexpr auto PulseFallKey     = "pulse_fall";
    constexpr auto NoiseBandsetKey  = "noise_bandset";
    constexpr auto NoiseStdevKey    = "noise_stdev";
    constexpr auto NoiseMeanKey     = "noise_mean";
    constexpr auto NoiseBandwidthKey = "noise_bandwidth";
    constexpr auto DcOffsetKey      = "dc_offset";
    constexpr auto DcPrecisionHighKey = "dc_precision_high";
    constexpr auto PolarityKey      = "polarity";
    constexpr auto OutputLoadKey    = "output_load";
    constexpr auto ExternalOutputKey = "external_output";
    constexpr auto MaxOutputAmplitudeKey = "max_output_amplitude";

    void channelCommonToJson(const ChannelState &state, QJsonObject &obj)
    {
        const auto & ampState = state.amplitude;

        obj[FrequencyKey]     = state.frequency;
        if (ampState.anyValid())
        {
            obj[AmplitudeKey] = ampState.valueRepresentation().value;
            obj[AmplitudeRepKey] =
                           ampState.valueRepresentation().representation;
            obj[AmplitudeUserRepKey] = ampState.userRepresentation;
            obj[OffsetKey]    = state.offset;
        }
        else
        {
            obj[VHighKey]     = state.vHigh;
            obj[VLowKey]      = state.vLow;
        }
        obj[PhaseKey]         = state.phase;
    }

    QJsonObject channelToJson(const ChannelState &state)
    {
        QJsonObject objBS;   // basic_settings:
        QString wf = state.waveform;

        if (wf == "SINE")
        {
            channelCommonToJson(state, objBS);
        }
        else if (wf == "SQUARE")
        {
            channelCommonToJson(state, objBS);
            objBS[DutyKey]           = state.duty;
        }
        else if (wf == "RAMP")
        {
            channelCommonToJson(state, objBS);
            objBS[SymmetryKey]       = state.rampSymmetry;
        }
        else if (wf == "PULSE")
        {
            channelCommonToJson(state, objBS);
            objBS[PulseWidthKey]     = state.pulseWidth;
            objBS[PulseRiseKey]      = state.pulseRise;
            objBS[PulseFallKey]      = state.pulseFall;
        }
        else if (wf == "NOISE")
        {
            objBS[NoiseBandsetKey]   = state.noiseBandset;
            objBS[NoiseStdevKey]     = state.noiseStdev;
            objBS[NoiseMeanKey]      = state.noiseMean;
            objBS[NoiseBandwidthKey] = state.noiseBandwidth;
        }
        else if (wf == "DC")
        {
            objBS[DcOffsetKey]       = state.dcOffset;
            objBS[DcPrecisionHighKey] = state.dcPrecisionHigh;
        }
        else
        {
            DEBUG_FUNC << "Unimplemented waveform:" << wf;
        }

        QJsonObject obj;

        obj[BasicSettingsKey]  = objBS;
        obj[PolarityKey]       =
                 Utility::polarityToString(state.output.polarity);
        obj[OutputLoadKey]     =
                 Utility::outputLoadToString(state.output.outputLoad);
        obj[ExternalOutputKey] = state.output.externalOutput;
        obj[WaveformKey]       = wf;
        obj[WaveformQualifierKey] = "none";
        return obj;
    }

    QJsonObject generalToJson(const GeneralState &gen_state)
    {
        QJsonObject obj;

        obj[ClockSourceKey]     =
                 Utility::clockSourceToString(gen_state.clockSource);

        obj[OverVoltageProtectionKey]     =
                 Utility::overVoltageProtectionToString(
                                  gen_state.overVoltageProtection);

        obj[SdgModeKey]     =
                 Utility::sdgModeToString(gen_state.sdgMode);
        return obj;
    }

    bool jsonToGeneral(const QJsonObject &obj, GeneralState &g_state)
    {
        const bool csPresent = obj.contains(ClockSourceKey);
        const bool ovpPresent = obj.contains(OverVoltageProtectionKey);
        const bool modePresent = obj.contains(SdgModeKey);

        if (!(csPresent || ovpPresent))
            return false;

        if (csPresent)
            g_state.clockSource =
               Utility::stringToClockSource(obj[ClockSourceKey].toString());
        if (ovpPresent)
            g_state.overVoltageProtection =
               Utility::stringToOverVoltageProtection(
                                 obj[OverVoltageProtectionKey].toString());
        if (modePresent)
            g_state.sdgMode =
               Utility::stringToSdgMode(obj[SdgModeKey].toString());
        return true;
    }

    void jsonCommonToChannel(ChannelState &state, QJsonObject &obj)
    {
        if (obj.contains(FrequencyKey))
            state.frequency = obj[FrequencyKey].toDouble();

        if (obj.contains(AmplitudeKey))
        {
            const double value = obj[AmplitudeKey].toDouble();
            AmplitudeState & ampState = state.amplitude;

            if (obj.contains(AmplitudeRepKey))
            {
                const QString r = obj[AmplitudeRepKey].toString();
                bool ok = true;

                /* Should be only normalized Units (i.e. no milliVolts) */
                if (r == "Vpp")
                    ampState.setAmplitudeVpp(value);
                else if (r == "Vrms")
                    ampState.setAmplitudeVrms(value);
                else if (r == "dBm")
                    ampState.setAmplitude_dBm(value);
                else if (! r.isEmpty()) // preserve empty userRep
                {
                    ok = false;
                    sdgDebug() << __func__ << ">>> rep=" << r;
                }
                if (ok && obj.contains(AmplitudeUserRepKey))
                    ampState.userRepresentation =
                         obj[AmplitudeUserRepKey].toString();
            }
        }
        else if (obj.contains(VHighKey))
        {
            state.vHigh = obj[VHighKey].toDouble();
            if (obj.contains(VLowKey))
                state.vLow = obj[VLowKey].toDouble();
        }

        if (obj.contains(PhaseKey))
            state.phase = obj[PhaseKey].toDouble();
    }

    bool jsonToChannel(const QJsonObject &obj, ChannelState &state)
    {

        if (!obj.contains(WaveformKey) ||
            !obj.contains(ExternalOutputKey) ||
            !obj.contains(BasicSettingsKey))
        {
            return false;
        }

        QJsonObject objBS { obj[BasicSettingsKey].toObject() };
        QString wf { obj[WaveformKey].toString() };

        state.waveform  = wf;
        state.output.polarity =
             Utility::stringToPolarity(obj[PolarityKey].toString());
        state.output.outputLoad =
             Utility::stringToOutputLoad(obj[OutputLoadKey].toString());

        state.output.externalOutput = obj[ExternalOutputKey].toBool();

        if (wf == "SINE")
        {
            jsonCommonToChannel(state, objBS);
        }
        else if (wf == "SQUARE")
        {
            jsonCommonToChannel(state, objBS);
            if (objBS.contains(DutyKey))
                state.duty = objBS[DutyKey].toDouble();
        }
        else if (wf == "RAMP")
        {
            jsonCommonToChannel(state, objBS);
            if (objBS.contains(SymmetryKey))
                state.rampSymmetry = objBS[SymmetryKey].toDouble();
        }
        else if (wf == "PULSE")
        {
            jsonCommonToChannel(state, objBS);
            if (objBS.contains(PulseWidthKey))
                state.pulseWidth = objBS[PulseWidthKey].toDouble();
            if (objBS.contains(PulseRiseKey))
                state.pulseRise = objBS[PulseRiseKey].toDouble();
            if (objBS.contains(PulseFallKey))
                state.pulseFall = objBS[PulseFallKey].toDouble();
        }
        else if (wf == "NOISE")
        {
            if (objBS.contains(NoiseBandsetKey))
                state.noiseBandset = objBS[NoiseBandsetKey].toBool();
            if (objBS.contains(NoiseStdevKey))
                state.noiseStdev = objBS[NoiseStdevKey].toDouble();
            if (objBS.contains(NoiseMeanKey))
                state.noiseMean = objBS[NoiseMeanKey].toDouble();
            if (objBS.contains(NoiseBandwidthKey))
                state.noiseBandwidth = objBS[NoiseBandwidthKey].toDouble();
        }
        else if (wf == "DC")
        {
            if (objBS.contains(DcOffsetKey))
                state.dcOffset = objBS[DcOffsetKey].toDouble();
            if (objBS.contains(DcPrecisionHighKey))
                state.dcPrecisionHigh = objBS[DcPrecisionHighKey].toBool();
        }

        return true;
    }
}

bool SettingsIO::save(const QString &filename,
                      const std::array<ChannelState, 2> &state,
                      const GeneralState &gen_state)
{
    QJsonObject root;
    QDateTime dt = QDateTime::currentDateTime();

    root[UtilityNameKey] = utilityName;
    root[UtilityVersionKey] = utilityVersion;
    root[ControlledInstrumentKey] = controlled_instrument;
    root[FormatVersionKey] = FormatVersion;
    root[DateTimeKey] = dt.toUTC().toString(Qt::ISODateWithMs);
    root[Channel1Key] = channelToJson(state.at(0));
    root[Channel2Key] = channelToJson(state.at(1));
    root[GeneralKey] = generalToJson(gen_state);

    QJsonDocument doc(root);

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        sdgDebug() << "Cannot open settings file for writing:"
                   << filename;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool SettingsIO::load(const QString &filename,
                      std::array<ChannelState, 2> &state,
                      GeneralState &gen_state,
                      QString *error)
{
    QFile file(filename);

    sdgDebug() << __func__;

    if (!file.open(QIODevice::ReadOnly))
    {
        if (error)
            *error = QString("Cannot open settings file: %1").arg(filename);
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        if (error)
            *error = parseError.errorString();
        return false;
    }

    if (!doc.isObject())
    {
        if (error)
            *error = "Settings file root is not a JSON object";
        return false;
    }

    const QJsonObject root = doc.object();

    // Now root["channel1"], root["channel2"], etc. are valid
    ChannelState ch1;
    ChannelState ch2;

    if (!jsonToChannel(root[Channel1Key].toObject(), ch1))
    {
        if (error)
            *error = "Invalid channel1 settings";
        return false;
    }

    if (!jsonToChannel(root[Channel2Key].toObject(), ch2))
    {
        if (error)
            *error = "Invalid channel2 settings";
        return false;
    }

    if (!jsonToGeneral(root[GeneralKey].toObject(), gen_state))
    {
        if (error)
            *error = "Invalid General settings";
        return false;
    }

    state[0] = ch1;
    state[1] = ch2;

    return true;
}
