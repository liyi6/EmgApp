#ifndef NETCONNECTHELPER_H
#define NETCONNECTHELPER_H

#include <QTcpSocket>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include "DataProcessThread.h"
#include "Defines.h"

class NetConnectHelper : public QObject
{
    Q_OBJECT

public:
    ~NetConnectHelper();
    static NetConnectHelper* instance();
    void conectToHost(QString ip, quint16 port);
    void disconnecToHost();
    void sendStartCmd();
    void sendStopCmd();
    void getDataContainer(QHash<int, QVector<double>*>& tmpDataContainer);

public slots:
    void netConnected();
    void netError(QAbstractSocket::SocketError);
    void netReadReady();

signals:
    void netConnected(bool connected);
    void dataComming(int channel, int data);

private:
    NetConnectHelper(QObject* parent = NULL);
    void clearDataProcessThread();

private:
    QTcpSocket             m_socket;
    DataProcessThread*     m_dataProcessThread;
    QMutex                 m_socketMutex;
};

#endif // NETCONNECTHELPER_H
