#pragma once

#include <QDateTime>
#include <QDebug>

/*
 * Like to have debug messages sent to the console prefixed with a
 * timestamp. Use: sdgDebug() << ...  instead of: qDebug << ...
 * Note that if SDG_DEBUG is not defined, change to No OPeration (NOP).
 */

#ifdef SDG_DEBUG
inline QDebug sdgDebug()
{
    return qDebug().noquote()
           << QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
}
#else

#define sdgDebug() if (true) {} else qDebug()

#endif

/* First attempt below using a macro. Replaced with inline function above */
#if 0
#ifdef SDG_DEBUG

#define SDG_LOG \
    qDebug().noquote() \
        << QDateTime::currentDateTime().toString("HH:mm:ss.zzz")

#else

#define SDG_LOG if (false) qDebug()

#endif
#endif
