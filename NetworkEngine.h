#ifndef NETWORKENGINE_H
#define NETWORKENGINE_H

#include <QTcpServer>
#include <QList>
#include <QVector>
#include "SocketWorker.h"
#include "NetworkTaskQueue.h"
#include <QHostAddress>
#include <QSet>
#include <QTimer>
#include "Transaction.h"

class Bingot;

class Peer
{
private:
    QHostAddress m_address;
    unsigned short m_port;

public:
    Peer(const QHostAddress &address = QHostAddress::Any, unsigned int port = 0);
    const QHostAddress &getAddress() const;
    unsigned short getPort() const;

    void operator=(const Peer &in);
    bool operator==(const Peer &in);

    friend bool operator==(const Peer &a, const Peer &b);
    friend uint qhash(const Peer &in);
    friend uint qHash(const Peer &key, uint seed);

};

bool operator==(const Peer &a, const Peer &b);

inline uint qhash(const Peer &in);

inline uint qHash(const Peer &key, uint seed);

class NetworkEngine : public QTcpServer
{
    Q_OBJECT
public:
    NetworkEngine();
    void incomingConnection(qintptr socketId);
    void initialize(Bingot *bingot);
    ~NetworkEngine();

    void addPeer(const Peer& in);

    void sendMessage(const QByteArray &msg);

private:
    QVector<SocketWorker*> m_socketWorkerList;
    NetworkTaskQueue *m_taskQueue;

    QMutex m_peerAddressesMutex;
    QSet<Peer> m_peerAddresses;

    QTimer m_peerTimer;

private slots:
    void onPeerTimerTimeout();
};

#endif // NETWORKENGINE_H
