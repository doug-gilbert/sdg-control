#pragma once

#include "Instrument.h"

enum class InstrumentType
{
    SDG2000X,
    Simulator
};

Instrument *createInstrument(InstrumentType type,
                             QObject *parent = nullptr);
