/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QString>

#include <array>

#include "GeneralState.h"
#include "ChannelState.h"


namespace SettingsIO
{
    bool save(const QString &filename,
              const std::array<ChannelState,2> &state,
              const GeneralState &gen_state);

    bool load(const QString &filename,
              std::array<ChannelState,2> &state,
              GeneralState &gen_state,
              QString *error = nullptr);
}
