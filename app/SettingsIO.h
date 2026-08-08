#pragma once

#include <QString>

#include <array>

#include "ChannelState.h"


namespace SettingsIO
{
    bool save(const QString &filename,
              const std::array<ChannelState,2> &state);

    bool load(const QString &filename,
              std::array<ChannelState,2> &state,
              QString *error = nullptr);
}
