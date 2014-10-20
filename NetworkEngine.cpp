#include "NetworkEngine.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Peer::Peer(const QHostAddress &address, unsigned int port)
    :m_address(address),
      m_port(port)
{

}

const QHostAddress &Peer::getAddress() const
{
    return m_address;
}

const unsigned short Peer::getPort() const
{
    return m_port;
}

void Peer::operator =(const Peer &in)
{
    m_address = in.m_address;
    m_port = in.m_port;
}

NetworkEngine::NetworkEngine()
    :QTcpServer(),
      m_neighborList(),
      m_socketWorkerList(),
      m_taskQueue(NULL)
{
}

void NetworkEngine::initialize()
{
    m_taskQueue = new NetworkTaskQueue();

    const unsigned int workerThreadCount = QThread::idealThreadCount();

    for(unsigned int i = 0; i < workerThreadCount; ++i)
    {
        SocketWorker *worker = new SocketWorker(m_taskQueue, this);
        worker->moveToThread(worker);
        m_socketWorkerList.push_back(worker);
        worker->start();
    }
}

void NetworkEngine::incomingConnection(qintptr socketId)
{
    ReceiveTask *receiveTask = new ReceiveTask(socketId);

    m_taskQueue->pushTask(receiveTask);
}

NetworkEngine::~NetworkEngine()
{
    foreach(SocketWorker *worker, m_socketWorkerList)
    {
        worker->stop();
        worker->deleteLater();
    }

    if (m_taskQueue)
    {
        delete m_taskQueue;
    }
}

QByteArray NetworkEngine::getPeerAddressJson()
{
    QJsonArray jarray;
    m_peerAddressesMutex.lock();

    foreach(Peer address, m_peerAddresses)
    {
        QJsonObject obj;
        obj["ip"] = address.getAddress().toString();
        obj["port"] = address.getPort();
        jarray.append(obj);
    }

    m_peerAddressesMutex.unlock();

    QJsonDocument jdoc(jarray);
    return jdoc.toJson();
}
