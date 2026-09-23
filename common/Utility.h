/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QString>

#include "GeneralState.h"
#include "ChannelState.h"

namespace Utility
{
    QString polarityToString(Polarity polarity);
    Polarity stringToPolarity(const QString &str);

    QString outputLoadToString(OutputLoad load);
    OutputLoad stringToOutputLoad(const QString &str);

    QString clockSourceToString(ClockSource source);
    ClockSource stringToClockSource(const QString &str);
}
