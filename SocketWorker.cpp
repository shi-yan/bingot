#include "SocketWorker.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include "NetworkEngine.h"

SocketWorker::SocketWorker(NetworkTaskQueue *taskQueue, NetworkEngine *engine)
    :QThread(),
      m_taskQueue(taskQueue),
      m_networkEngine(engine),
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
            sendMessage((SendTask*) task);
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
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(task->getIp(), task->getPort());
    connect(m_socket, SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));
    m_socket->waitForConnected();

    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->write(task->getMessage());
    }

    delete task;
    waitForTaskFinish();
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
    QJsonParseError error;
    QJsonDocument jdoc;

    jdoc.fromJson(json, &error);

    if (error.error == QJsonParseError::NoError)
    {
        if (jdoc.isObject())
        {
            QJsonObject obj = jdoc.object();

            if (obj.contains("message"))
            {
                QString message = obj["message"].toString();

                if(message == "PeerSync")
                {
                    QJsonArray peers = obj["peers"].toArray();

                    foreach(QJsonValue val, peers)
                    {
                        QJsonObject pObj = val.toObject();

                        QHostAddress ip( pObj["ip"].toString());
                        unsigned short port = pObj["port"].toInt();

                        Peer p(ip,port);

                        m_networkEngine->addPeer(p);
                    }
                }
                else if(message == "Transaction")
                {
                    Transaction t;
                    if (t.parseFromJsonObject(obj))
                    {
                        emit newTransaction(t);
                    }
                }
                else if(message == "Block")
                {
                    Block b;
                    if (b.parseFromQJsonObject(obj))
                    {
                        emit newBlock(b);
                    }
                }
                else if(message == "QueryBlock")
                {
                    int index = obj["index"].toInt();
                    emit queryBlock(index);
                }
                else if(message == "QueryBlockChainLength")
                {
                    emit queryBlockChainLength();
                }
                else if(message == "BlockChainLength")
                {
                    int length = obj["length"].toInt();
                    emit getBlockChainLength(length);
                }
            }
            else
            {
                qDebug() << "ill-formed json message";
            }
        }
        else
        {
            qDebug() << "ill-formed json message";
        }
    }
    else
    {
        qDebug() << "ill-formed json message";

    }
}
