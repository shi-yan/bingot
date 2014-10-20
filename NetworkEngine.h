#ifndef NETWORKENGINE_H
#define NETWORKENGINE_H

#include <QTcpServer>
#include <QList>
#include <QVector>
#include "Neighbor.h"
#include "SocketWorker.h"
#include "NetworkTaskQueue.h"
#include <QHostAddress>
#include <QSet>

class Peer
{
private:
    QHostAddress m_address;
    unsigned short m_port;

public:
    Peer(const QHostAddress &address = QHostAddress::Any, unsigned int port = 0);
    const QHostAddress &getAddress() const;
    const unsigned short getPort() const;

    void operator=(const Peer &in);
};

class NetworkEngine : public QTcpServer
{
    Q_OBJECT
public:
    NetworkEngine();
    void incomingConnection(qintptr socketId);
    void initialize();
    ~NetworkEngine();

    QByteArray getPeerAddressJson();

private:
    QList<Neighbor> m_neighborList;
    QVector<SocketWorker*> m_socketWorkerList;
    NetworkTaskQueue *m_taskQueue;

    QMutex m_peerAddressesMutex;
    QSet<Peer> m_peerAddresses;
};

#endif // NETWORKENGINE_H
