#ifndef DATAPROCESSTHREAD_H
#define DATAPROCESSTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

#include "Defines.h"

class DataProcessThread : public QThread
{
    Q_OBJECT
public:
    explicit DataProcessThread(QTcpSocket* socket, QMutex* socketMutex);
    ~DataProcessThread();

    void run();
    void stop();

    void getDataContainer(QHash<int, QVector<short>>& tmpDataContainer);
signals:
    void dataComming(int channel, int data);

private:
    void dataProcess(QByteArray& data);

private:
    volatile bool m_stop;
    volatile bool m_headFind;
    QTcpSocket*   m_socket;
    QMutex*       m_socketMutex;
    int           m_curChennel;    // 当前通道
    QByteArray    m_dataLeft;      // dataLeft为上次处理后的余留数据(当数据)
    QHash<int, QVector<short>>  m_dataContainer;
    QMutex        m_dataContainerMutex;
};

#endif // DATAPROCESSTHREAD_H
