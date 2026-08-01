#pragma once

#include <QTcpSocket>
#include <QString>

class ScpiConnection
{
public:
    bool connectTo(const QString& host, quint16 port = 5025);

    QString query(const QString& command);

    bool command(const QString& command);

    bool isConnected() const;

private:
    QTcpSocket socket;
};
