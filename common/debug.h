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

// Quiet version. So no leading date-time stamp
inline QDebug qsdgDebug()
{
    return qDebug().noquote();
}

// QDebug() is crippled, it automatically appends a trailing NL (new line:
// linefeed LF) character whether or not it is wanted. There is NO way to
// disable this stupid behaviour in Qt6. QTestStream is not copyable, fail.

#else

#define sdgDebug() if (true) {} else qDebug()
#define qsdgDebug() if (true) {} else qDebug()

#endif
