/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

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

    QString polarityToString(Polarity polarity);
    Polarity stringToPolarity(const QString &str);
    QString outputLoadToString(OutputLoad load);
    OutputLoad stringToOutputLoad(const QString &str);
}
