/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QString>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "Utility.h"
#include "debug.h"


QString Utility::polarityToString(Polarity polarity)
{
    switch (polarity) {
    case Polarity::Normal:
        return "Normal";
    case Polarity::Inverted:
        return "Inverted";
    }
    return "Normal";    // defensive fallback
}

Polarity Utility::stringToPolarity(const QString &str)
{
    if (str == "Inverted")
        return Polarity::Inverted;
    return Polarity::Normal;
}

QString Utility::outputLoadToString(OutputLoad load)
{
    switch (load) {
    case OutputLoad::Ohms50:
        return "50";
    case OutputLoad::HiZ:
        return "HiZ";
    }
    return "50";
}

OutputLoad Utility::stringToOutputLoad(const QString &str)
{
    if (str == "HiZ")
        return OutputLoad::HiZ;
    return OutputLoad::Ohms50;
}

QString Utility::clockSourceToString(ClockSource refClk)
{
    switch (refClk) {
    case ClockSource::Internal:
        return "Internal";
    case ClockSource::External:
        return "External";
    }
    return "Internal";
}

ClockSource Utility::stringToClockSource(const QString &str)
{
    if (str == "Internal")
        return ClockSource::Internal;
    return ClockSource::External;
}
