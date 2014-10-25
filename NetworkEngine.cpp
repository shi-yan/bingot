#include "NetworkEngine.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "Bingot.h"

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

void NetworkEngine::initialize(Bingot *bingot)
{
    m_taskQueue = new NetworkTaskQueue();

    const unsigned int workerThreadCount = QThread::idealThreadCount();

    for(unsigned int i = 0; i < workerThreadCount; ++i)
    {
        SocketWorker *worker = new SocketWorker(m_taskQueue, this);
        connect(worker, SIGNAL(newTransaction(Transaction)), bingot, SLOT(onNewTransaction(Transaction)));
        connect(worker, SIGNAL(newBlock(Block)), bingot, SLOT(onNewBlockReceived(Block)));
        connect(worker, SIGNAL(queryBlock(int)), bingot, SLOT(onQueryBlock(int)));
        connect(worker, SIGNAL(queryBlockChainLength()),bingot,SLOT(onQueryBlockChainLength()));
        connect(worker, SIGNAL(getBlockChainLength(int)),bingot,SLOT(onReceivedBlockChainLength(int)));
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

void NetworkEngine::onPeerTimerTimeout()
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

    jobj["message"] = "PeerSync";
    jobj["peers"] = jarray;

    QJsonDocument jdoc(jobj);

    sendMessage(jdoc.toJson());
}

void NetworkEngine::addPeer(const Peer& in)
{
    m_peerAddressesMutex.lock();

    m_peerAddresses.insert(in);

    m_peerAddressesMutex.unlock();
}

void NetworkEngine::sendMessage(const QByteArray &msg)
{
    QByteArray data;

    data.append((quint32) msg.size());
    data.append(msg);

    m_peerAddressesMutex.lock();
    foreach(Peer peer, m_peerAddresses)
    {
        SendTask *task = new SendTask(data, peer.getAddress(), peer.getPort());
        m_taskQueue->pushTask(task);
    }
    m_peerAddressesMutex.unlock();
}
