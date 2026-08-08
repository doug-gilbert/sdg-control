
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

#include "InstrumentFactory.h"

#include "SDG2000X.h"
#include "SimulatorInstrument.h"

Instrument *createInstrument(InstrumentType type,
                             QObject *parent)
{
    switch (type)
    {
    case InstrumentType::SDG2000X:
        return new SDG2000X(parent);

    case InstrumentType::Simulator:
        return new SimulatorInstrument(parent);
    }

    return nullptr;
}

