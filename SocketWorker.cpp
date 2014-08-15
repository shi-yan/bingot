#include "SocketWorker.h"
#include <QCoreApplication>
#include <QDebug>

SocketWorker::SocketWorker(NetworkTaskQueue *taskQueue)
    :QThread(),
      m_taskQueue(taskQueue),
      m_hasTaskFinished(false),
      m_stop(false),
      m_socket(NULL),
      m_incommingMessageSize(-1),
      m_tempDataBuffer()
{
}

void SocketWorker::waitForTaskFinish()
{
    while(!m_hasTaskFinished)
    {
        QCoreApplication::processEvents();
    }

    if (m_socket)
    {
        m_socket->deleteLater();
        m_socket = NULL;
    }
}

void SocketWorker::run()
{
    while(!m_stop)
    {
        NetworkTask *task = m_taskQueue->popTask();

        m_tempDataBuffer.clear();
        m_hasTaskFinished = false;
        m_incommingMessageSize = -1;

        switch(task->getType())
        {
        case NetworkTask::SEND:
            break;
        case NetworkTask::RECEIVE:
            receiveMessage((ReceiveTask*)task);
            break;
        default:
            qDebug() << "unimplemented network task:" << task->getType();
        }
    }
}

void SocketWorker::stop()
{
    m_stop = true;
}

void SocketWorker::receiveMessage(ReceiveTask *task)
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

    m_socket->setSocketDescriptor(task->getSocketId());

    delete task;
    waitForTaskFinish();

}

void SocketWorker::sendMessage(SendTask *task)
{

}

void SocketWorker::onSocketReadyRead()
{
    m_tempDataBuffer.append(m_socket->readAll());
    if (m_incommingMessageSize == -1)
    {
        if (m_tempDataBuffer.size() > sizeof(quint32))
        {
            //little endian only for now
            m_incommingMessageSize = *((quint32*) m_tempDataBuffer.data());
        }
    }

    if (m_incommingMessageSize != -1 && m_tempDataBuffer.size() >= (sizeof(quint32) + m_incommingMessageSize))
    {
        handleIncommingMessage(m_tempDataBuffer.mid(sizeof(quint32)));
    }
}

void SocketWorker::onSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error:" << error;
    m_hasTaskFinished = true;
}

void SocketWorker::onSocketConnected()
{

}

void SocketWorker::onSocketDisconnected()
{

}

void SocketWorker::handleIncommingMessage(QByteArray json)
{

}
