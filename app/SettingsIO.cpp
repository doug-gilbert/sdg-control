
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

    constexpr auto WaveformKey  = "waveform";
    constexpr auto FrequencyKey = "frequency";
    constexpr auto AmplitudeKey = "amplitude";
    constexpr auto OffsetKey    = "offset";
    constexpr auto PhaseKey     = "phase";
    constexpr auto SymmetryKey  = "symmetry";
    constexpr auto OutputKey    = "output";

    QJsonObject channelToJson(const ChannelState &state)
    {
        QJsonObject obj;

        obj[WaveformKey]  = state.waveform;
        obj[FrequencyKey] = state.frequency;
        obj[AmplitudeKey] = state.amplitude;
        obj[OffsetKey]    = state.offset;
        obj[PhaseKey]     = state.phase;
        obj[SymmetryKey]  = state.symmetry;
        obj[OutputKey]    = state.output;

        return obj;
    }

    bool jsonToChannel(const QJsonObject &obj, ChannelState &state)
    {
        if (!obj.contains(WaveformKey) ||
            !obj.contains(FrequencyKey) ||
            !obj.contains(AmplitudeKey) ||
            !obj.contains(OffsetKey) ||
            !obj.contains(PhaseKey) ||
            !obj.contains(OutputKey))
        {
            return false;
        }

        state.waveform  = obj[WaveformKey].toString();
        state.frequency = obj[FrequencyKey].toDouble();
        state.amplitude = obj[AmplitudeKey].toDouble();
        state.offset    = obj[OffsetKey].toDouble();
        state.phase     = obj[PhaseKey].toDouble();
        state.symmetry  = obj[SymmetryKey].toDouble();
        state.output    = obj[OutputKey].toBool();

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
