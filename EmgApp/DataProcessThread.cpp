#include "DataProcessThread.h"

DataProcessThread::DataProcessThread(QTcpSocket* socket, QMutex* socketMutex)
    : m_stop(false),
      m_socket(socket),
      m_socketMutex(socketMutex),
      m_headFind(false),
      m_curChennel(0)
{
    if (!socket || !socketMutex) {
        qWarning() << "socket and socketMutex can't be null.";
    }

    for (int channel=0; channel<CHANNEL_SIZE; channel++) {
        // set data container
        QVector<double>* dataList = new QVector<double>();
        m_dataContainer.insert(channel, dataList);
    }

    m_dataLeft.clear();
}

DataProcessThread::~DataProcessThread()
{
    qDebug() << "~DataProcessThread() called.";
}

void DataProcessThread::run()
{
    QDir recordFileDir;
    recordFileDir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    QString recordFilePath = QString(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + "emg-%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + ".dat");
    QFile recordFile(recordFilePath);
    qDebug() << "Create a new record file, path:" << recordFilePath;
    if (!recordFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) {
        qWarning() << "Fail to Open record file.";
        return;
    }

    while (!m_stop) {
        if (m_socket && m_socket->bytesAvailable()) {
            QByteArray data;
            {
                QMutexLocker locker(m_socketMutex);
                data = m_socket->read(4096);
                //qDebug() << "recv data->" << data;
            }

            if (!m_headFind) {     // 如果还未读到数据头
                int head = data[0] & 0x000000FF;
                head |= ((data[1] << 8) & 0x0000FF00);
                head |= ((data[2] << 16) & 0x00FF0000);
                head |= ((data[3] << 24) & 0xFF000000);
                if (0xAAAA5555 == head) {
                    qDebug() << "Success to find head of data.";
                    m_headFind = true;
                    data.remove(0, 4);
                    recordFile.write(data);
                    dataProcess(data);

                } else {
                    qWarning() << "Head of data find failed, data will be bannished";
                }
            } else {
                recordFile.write(data);
                dataProcess(data);
            }
        }
    }

    recordFile.close();
}

void DataProcessThread::stop()
{
    m_stop = true;
}

void DataProcessThread::getDataContainer(QHash<int, QVector<double>*>& tmpDataContainer)
{
    {
        QVector<double>* tmpVec;
        QVector<double>* srcVec;
        QMutexLocker locker(&m_dataContainerMutex);
        for (int channel=0; channel<CHANNEL_SIZE; channel++) {
             tmpVec = tmpDataContainer.value(channel);
             srcVec = m_dataContainer.value(channel);
//            while (!srcVec->isEmpty()) {
//                double data = (double)srcVec->takeLast();
//                tmpVec->push_front(data);
//            }
            tmpVec->append(*srcVec);
            srcVec->clear();
            //qDebug() << "Channel" << channel << "tmpVec size->" << tmpVec->size();
        }

    }
}

void DataProcessThread::dataProcess(QByteArray &data)
{
//    /**
//     * channel数为16(0~15), 每个通道数据占4个Byte
//     * 若某次传入的data的Byte数不为4的倍数时会有余留数据，需要保存到下次处理
//     */

//    if (m_dataLeft.size() > 0) {
//        data.prepend(m_dataLeft);
//    }

//    while(data.size() >= 4) {
//        int channelData = data[0] & 0x000000FF;
//        channelData |= ((data[1] << 8) & 0x0000FF00);
//        channelData |= ((data[2] << 16) & 0x00FF0000);
//        channelData |= ((data[3] << 24) & 0xFF000000);
//        emit dataComming(m_curChennel, channelData);
//        m_curChennel = ++m_curChennel % 16;
//        data = data.mid(4);
//        if (data.size() < 4 && data.size() > 0) {
//            m_dataLeft = data;
//        }
//    }

    /**
     * channel数为16(0~15), 每个通道数据占2个Byte
     * 若某次传入的data的Byte数不为偶数时会有余留数据，需要保存到下次处理
     */

    if (m_dataLeft.size() > 0) {
        data.prepend(m_dataLeft);
    }

    while(data.size() > 1) {
        short channelData = data[0] & 0x000000FF;
        channelData |= ((data[1] << 8) & 0x0000FF00);

        {
            QMutexLocker locker(&m_dataContainerMutex);
            QVector<double>* dataVector = m_dataContainer.value(m_curChennel);
            dataVector->append(channelData);
            if (dataVector->size() > 24000) {  // 丢弃超过24000的数据点
                qDebug() << "Channle" <<m_curChennel <<"ori vec size is " <<dataVector->size();
                dataVector->remove(0, dataVector->size()-24000);
            }
        }

        m_curChennel = ++m_curChennel % CHANNEL_SIZE;
        data.remove(0,2);
    }

    if (data.size() == 1) {
        m_dataLeft = data;
    }
}
