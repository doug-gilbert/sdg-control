#include "ScpiConnection.h"

#include <QThread>
#ifdef DEBUG
#include <QDebug>
#endif

bool ScpiConnection::connectTo(const QString& host, quint16 port)
{
    socket.connectToHost(host, port);

    return socket.waitForConnected(3000);
}


QString ScpiConnection::query(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

    socket.write(cmd);

    if (!socket.waitForBytesWritten(1000))
        return "WRITE ERROR";

    if (!socket.waitForReadyRead(3000))
        return "READ TIMEOUT";

    QByteArray reply;

    while (socket.bytesAvailable())
    {
        reply += socket.readAll();

        // Allow the instrument time to finish the response
        socket.waitForReadyRead(100);
    }

    return QString::fromUtf8(reply).trimmed();
}

bool ScpiConnection::command(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

#ifdef SDG_DEBUG
    qDebug() << "TX command:" << cmd;
#endif

    socket.write(cmd);

    if (!socket.waitForBytesWritten(1000))
        return false;

    // Wait until the SDG TCP stack has accepted the command
    while (socket.waitForReadyRead(50))
    {
        socket.readAll();
    }

    return true;
}

#if 0
bool ScpiConnection::command(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

    qDebug() << "TX command:" << cmd;

    socket.write(cmd);
    socket.flush();

    if (!socket.waitForBytesWritten(1000))
        return false;

    // See if the instrument sends anything back
    if (socket.waitForReadyRead(500))
    {
        QByteArray reply = socket.readAll();
        qDebug() << "Unexpected RX:" << reply;
    }

    return true;
}

bool ScpiConnection::command(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

    socket.write(cmd);
    socket.flush();

    if (!socket.waitForBytesWritten(1000))
        return false;

    // Allow instrument time to process the command
    QThread::msleep(100);

    return true;
}

bool ScpiConnection::command(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

    socket.write(cmd);

    return socket.waitForBytesWritten(1000);
}
#endif


bool ScpiConnection::isConnected() const
{
    return socket.state() == QAbstractSocket::ConnectedState;
}
