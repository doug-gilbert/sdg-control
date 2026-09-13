/*
 * Copyright (c) 2026 Douglas Gilbert.
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Instrument.h"

enum class InstrumentType
{
    SDG2000X,
    Simulator
};

Instrument *createInstrument(InstrumentType type,
                             QObject *parent = nullptr);
