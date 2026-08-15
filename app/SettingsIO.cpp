
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "SettingsIO.h"
#include "debug.h"

namespace
{
    constexpr int FormatVersion = 1;

    constexpr auto FormatVersionKey = "formatVersion";
    constexpr auto Channel1Key      = "channel1";
    constexpr auto Channel2Key      = "channel2";

    constexpr auto WaveformKey      = "waveform";
    constexpr auto FrequencyKey     = "frequency";
    constexpr auto AmplitudeKey     = "amplitude";
    constexpr auto AmplitudeRepresentationKey = "amplitudeRepresentation";
    constexpr auto OffsetKey        = "offset";
    constexpr auto PhaseKey         = "phase";
    constexpr auto SymmetryKey      = "rampSymmetry";
    constexpr auto PulseWidthKey    = "pulseWidth";
    constexpr auto PulseRiseKey     = "pulseRise";
    constexpr auto PulseFallKey     = "pulseFall";
    constexpr auto NoiseBandsetKey  = "noiseBandset";
    constexpr auto NoiseStdevKey    = "noiseStdev";
    constexpr auto NoiseMeanKey     = "noiseMean";
    constexpr auto NoiseBandwidthKey = "noiseBandwidth";
    constexpr auto DcOffsetKey      = "dcOffset";
    constexpr auto DcPrecisionHighKey = "dcPrecisionHigh";

    constexpr auto OutputKey    = "output";

    QJsonObject channelToJson(const ChannelState &state)
    {
        QJsonObject obj;
        const auto representation = state.amplitude.userRepresentation();


        obj[WaveformKey]       = state.waveform;
        obj[FrequencyKey]      = state.frequency;
        obj[AmplitudeKey]      = state.amplitude.userValue();
        obj[AmplitudeRepresentationKey] =
            representation == SdgAmplitude::Representation::Vpp  ? "Vpp" :
            representation == SdgAmplitude::Representation::Vrms ? "Vrms" :
                                                                   "dBm";
        obj[OffsetKey]         = state.offset;
        obj[PhaseKey]          = state.phase;
        obj[SymmetryKey]       = state.rampSymmetry;
        obj[PulseWidthKey]     = state.pulseWidth;
        obj[PulseRiseKey]      = state.pulseRise;
        obj[PulseFallKey]      = state.pulseFall;
        obj[NoiseBandsetKey]   = state.noiseBandset;
        obj[NoiseStdevKey]     = state.noiseStdev;
        obj[NoiseMeanKey]      = state.noiseMean;
        obj[NoiseBandwidthKey] = state.noiseBandwidth;
        obj[DcOffsetKey]       = state.dcOffset;
        obj[DcPrecisionHighKey] = state.dcPrecisionHigh;

        obj[OutputKey]         = state.output;

        return obj;
    }

    bool jsonToChannel(const QJsonObject &obj, ChannelState &state)
    {
        if (!obj.contains(WaveformKey) ||
            !obj.contains(OutputKey))
        {
            return false;
        }

        state.waveform  = obj[WaveformKey].toString();
        state.output    = obj[OutputKey].toBool();

        if (obj.contains(FrequencyKey))
            state.frequency = obj[FrequencyKey].toDouble();
        if (obj.contains(AmplitudeKey))
        {
            const double value = obj[AmplitudeKey].toDouble();

            SdgAmplitude::Representation representation =
                SdgAmplitude::Representation::Vpp;

            if (obj.contains(AmplitudeRepresentationKey))
            {
                const QString r = obj[AmplitudeRepresentationKey].toString();

                if (r == "Vrms")
                    representation = SdgAmplitude::Representation::Vrms;
                else if (r == "dBm")
                    representation = SdgAmplitude::Representation::dBm;
            }

            state.amplitude.setUserValue(value, representation);
        }
        if (state.waveform == "DC")
        {
            if (obj.contains(DcOffsetKey))
                state.dcOffset = obj[DcOffsetKey].toDouble();

            if (obj.contains(DcPrecisionHighKey))
                state.dcPrecisionHigh = obj[DcPrecisionHighKey].toBool();
        }
        else
        {
            if (obj.contains(OffsetKey))
                state.offset = obj[OffsetKey].toDouble();
        }
        if (obj.contains(PhaseKey))
            state.phase = obj[PhaseKey].toDouble();
        if (obj.contains(SymmetryKey))
            state.rampSymmetry = obj[SymmetryKey].toDouble();
        if (obj.contains(PulseWidthKey))
            state.pulseWidth = obj[PulseWidthKey].toDouble();
        if (obj.contains(PulseRiseKey))
            state.pulseRise = obj[PulseRiseKey].toDouble();
        if (obj.contains(PulseFallKey))
            state.pulseFall = obj[PulseFallKey].toDouble();
        if (obj.contains(NoiseBandsetKey))
            state.noiseBandset = obj[NoiseBandsetKey].toBool();
        if (obj.contains(NoiseStdevKey))
            state.noiseStdev = obj[NoiseStdevKey].toDouble();
        if (obj.contains(NoiseMeanKey))
            state.noiseMean = obj[NoiseMeanKey].toDouble();
        if (obj.contains(NoiseBandwidthKey))
            state.noiseBandwidth = obj[NoiseBandwidthKey].toDouble();

        return true;
    }
}

bool SettingsIO::save(const QString &filename,
                      const std::array<ChannelState, 2> &state)
{
    QJsonObject root;

    root[FormatVersionKey] = FormatVersion;
    root[Channel1Key] = channelToJson(state.at(0));
    root[Channel2Key] = channelToJson(state.at(1));

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

    QJsonObject root = doc.object();

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

    state[0] = ch1;
    state[1] = ch2;

    return true;
}
