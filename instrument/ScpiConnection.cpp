#include "ScpiConnection.h"

#include <QThread>
#include <QAbstractSocket>

#ifdef SDG_DEBUG
#include <QDebug>
#endif


bool ScpiConnection::connectTo(const QString& host, quint16 port)
{
    if (socket.state() != QAbstractSocket::UnconnectedState)
    {
        socket.abort();
    }

    socket.connectToHost(host, port);

    if (!socket.waitForConnected(3000))
    {
        qDebug() << "Connection failed:"
                 << socket.errorString();

        socket.abort();
        return false;
    }

    return true;
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


bool ScpiConnection::isConnected() const
{
    return socket.state() == QAbstractSocket::ConnectedState;
}

QString ScpiConnection::errorString() const
{
    return socket.errorString();
}
