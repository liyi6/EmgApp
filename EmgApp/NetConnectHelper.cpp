#include "NetConnectHelper.h"

NetConnectHelper::NetConnectHelper(QObject *)
    : m_dataProcessThread(NULL)
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(netConnected()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(netError(QAbstractSocket::SocketError)));
}

void NetConnectHelper::clearDataProcessThread()
{
    if (m_dataProcessThread && m_dataProcessThread->isRunning()) {
        m_dataProcessThread->stop();
        m_dataProcessThread->wait(2000);
    }

    if (m_dataProcessThread && m_dataProcessThread->isRunning()) {
        m_dataProcessThread->terminate();
    }

    delete m_dataProcessThread;
    m_dataProcessThread = NULL;
}

NetConnectHelper::~NetConnectHelper()
{
    disconnecToHost();
}

NetConnectHelper* NetConnectHelper::instance()
{
    static NetConnectHelper instance;
    return &instance;
}

void NetConnectHelper::conectToHost(QString ip, quint16 port)
{
    if (QAbstractSocket::UnconnectedState != m_socket.state()) {
        {
            QMutexLocker locker(&m_socketMutex);
            m_socket.close();
        }
    }

    {
        QMutexLocker locker(&m_socketMutex);
        m_socket.connectToHost(ip, port);
    }
}

void NetConnectHelper::disconnecToHost()
{
    if (QAbstractSocket::UnconnectedState != m_socket.state()) {
        {
            QMutexLocker locker(&m_socketMutex);
            m_socket.close();
        }
    }
}

void NetConnectHelper::sendStartCmd()
{
    clearDataProcessThread();

    if (!m_dataProcessThread) {
        m_dataProcessThread = new DataProcessThread(&m_socket, &m_socketMutex);
        m_dataProcessThread->start();
    }

    if (QAbstractSocket::ConnectedState == m_socket.state()) {
        Command cmd;
        cmd.header      = 0x5555AAAA;
        cmd.cmdType     = 0x00000001;

        {
            QMutexLocker locker(&m_socketMutex);
            m_socket.write(cmd.toByteArray());
        }

        qDebug() << "Send start cmd: " << cmd.toByteArray();
    } else {
        qWarning() << "Fail to send start cmd because of disconnecting to server.";
    }
}

void NetConnectHelper::sendStopCmd()
{
    if (QAbstractSocket::ConnectedState == m_socket.state()) {
        Command cmd;
        cmd.header      = 0x5555AAAA;
        cmd.cmdType     = 0x00000000;

        {
            QMutexLocker locker(&m_socketMutex);
            m_socket.write(cmd.toByteArray());
        }

        qDebug() << "Send stop cmd: " << cmd.toByteArray();

    } else {
        qWarning() << "Fail to send stop cmd because of disconnecting to server.";
    }

    clearDataProcessThread();
}

void NetConnectHelper::getDataContainer(QHash<int, QVector<double>*> &tmpDataContainer)
{
    if (m_dataProcessThread) {
        return m_dataProcessThread->getDataContainer(tmpDataContainer);
    }
}

void NetConnectHelper::netConnected()
{
    emit netConnected(true);
}

void NetConnectHelper::netError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error occur: " << error;
    emit netConnected(false);
}



