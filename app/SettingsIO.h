#include <QString>

#include <array>

#include "SDG2000X.h"


namespace SettingsIO
{
    bool save(const QString &filename,
              const std::array<ChannelState,2> &state);

    bool load(const QString &filename,
              std::array<ChannelState,2> &state,
              QString *error = nullptr);
}
