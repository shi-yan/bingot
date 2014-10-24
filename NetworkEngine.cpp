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

unsigned short Peer::getPort() const
{
    return m_port;
}

void Peer::operator =(const Peer &in)
{
    m_address = in.m_address;
    m_port = in.m_port;
}

bool Peer::operator==(const Peer &in)
{
    return (in.m_address == m_address) && (in.m_port == m_port);
}

bool operator==(const Peer &a, const Peer &b)
{
    return (a.m_address == b.m_address) && (a.m_port == b.m_port);
}

uint qhash(const Peer &in)
{
    return in.m_address.toIPv4Address();
}

uint qHash(const Peer &key, uint seed)
{
    return key.m_address.toIPv4Address() ^ seed;
}

NetworkEngine::NetworkEngine()
    :QTcpServer(),
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

    connect(&m_peerTimer, SIGNAL(timeout()), this, SLOT(onPeerTimerTimeout()));
    m_peerTimer.start(600000);
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

    QJsonObject jobj;

    jobj["message"] = "PeerSyncReply";
    jobj["peers"] = jarray;

    QJsonDocument jdoc(jobj);
    return jdoc.toJson();
}

void NetworkEngine::onPeerTimerTimeout()
{
    qDebug() << "query for new peers";

    QByteArray message;

    QJsonObject obj;
    obj["message"] = "PeerSync";
    QJsonDocument jdoc(obj);

    message.append(jdoc.toJson());
    message.prepend((quint32) message.size());

    m_peerAddressesMutex.lock();
    foreach(Peer peer, m_peerAddresses)
    {
        SendTask *task = new SendTask(message, peer.getAddress(), peer.getPort());
        m_taskQueue->pushTask(task);
    }
    m_peerAddressesMutex.unlock();

}

void NetworkEngine::addPeer(const Peer& in)
{
    m_peerAddressesMutex.lock();

    m_peerAddresses.insert(in);

    m_peerAddressesMutex.unlock();
}
