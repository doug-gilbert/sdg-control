
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

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
    sdgDebug() << __func__;

    // placeholder ...
    Q_UNUSED(filename);
    Q_UNUSED(state);
    Q_UNUSED(error);
    return true;
}
