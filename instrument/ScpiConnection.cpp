
#include <QThread>
#include <QAbstractSocket>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef SDG_DEBUG
#warning "config.h file NOT found"
#endif
#endif

/* Include config.h _before_ any local includes in case they need it */

#include "ScpiConnection.h"
#include "debug.h"      /* in common sub-directory */


ScpiConnection::ScpiConnection(QObject *parent)
    : QObject(parent)
{
    connect(&socket,
            &QTcpSocket::disconnected,
            this,
            &ScpiConnection::disconnected);

    connect(&socket,
            &QTcpSocket::errorOccurred,
            this,
            [this](QAbstractSocket::SocketError)
            {
                emit connectionError(socket.errorString());
            });
}

ScpiConnection::~ScpiConnection()
{
    sdgDebug() << "ScpiConnection destructor";

    if (socket.state() != QAbstractSocket::UnconnectedState)
    {
        socket.disconnectFromHost();

        if (socket.state() != QAbstractSocket::UnconnectedState)
            socket.waitForDisconnected(1000);
    }
}

bool ScpiConnection::connectTo(const QString& host, quint16 port)
{
    if (socket.state() != QAbstractSocket::UnconnectedState)
    {
        socket.abort();
    }

    socket.connectToHost(host, port);

    if (!socket.waitForConnected(3000))
    {
        sdgDebug() << "Connection failed:"
                   << socket.errorString();

        socket.abort();
        return false;
    }

    return true;
}

void ScpiConnection::disconnect()
{
    socket.abort();
}

QString ScpiConnection::query(const QString& command)
{
    QByteArray cmd = command.toUtf8() + "\r\n";

    socket.write(cmd);

    if (!socket.waitForBytesWritten(1000))
    {
        socket.abort();
        emit disconnected();
        return "WRITE ERROR";
    }

    if (!socket.waitForReadyRead(3000))
    {
        socket.abort();
        emit disconnected();
        return "READ TIMEOUT";
    }

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
    if (!isConnected())
        return false;

    sdgDebug() << "TX command:" << command;
    QByteArray cmd = command.toUtf8() + "\r\n";

    socket.write(cmd);

    if (!socket.waitForBytesWritten(1000))
    {
        socket.abort();
        emit disconnected();
        return false;
    }

#if 0
    // Wait until the SDG TCP stack has accepted the command
    while (socket.waitForReadyRead(50))
    {
        socket.readAll();
    }
#endif

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
