
#include <QObject>

#include "Instrument.h"
#include "SDG2000X.h"

Instrument *createInstrument(QObject *parent)
{
    return new SDG2000X(parent);
}
