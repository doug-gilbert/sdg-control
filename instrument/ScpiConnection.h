#pragma once

#include <QTcpSocket>
#include <QString>
#include <QByteArray>


class ScpiConnection : public QObject
{
    Q_OBJECT
  
public:
    explicit ScpiConnection(QObject *parent = nullptr);

    ~ScpiConnection();

    bool connectTo(const QString& host, quint16 port = 5025);

    void disconnect();

    QString query(const QString& command, int timeout = 3000);
    QByteArray queryBinary(const QString& command);

    bool command(const QString& command);

    bool waitForOperationComplete(int timeout_ms = 10000);

    bool isConnected() const;

    QString errorString() const;

signals:
    void disconnected();
    void connectionError(const QString &message);

private:
    QTcpSocket socket;
};
