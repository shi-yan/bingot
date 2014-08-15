#ifndef NETWORKENGINE_H
#define NETWORKENGINE_H

#include <QTcpServer>
#include <QList>
#include <QVector>
#include "Neighbor.h"
#include "SocketWorker.h"
#include "NetworkTaskQueue.h"

class NetworkEngine : public QTcpServer
{
    Q_OBJECT
public:
    NetworkEngine();
    void incomingConnection(qintptr socketId);
    void initialize();
    ~NetworkEngine();
private:
    QList<Neighbor> m_neighborList;
    QVector<SocketWorker*> m_socketWorkerList;
    NetworkTaskQueue *m_taskQueue;
};

#endif // NETWORKENGINE_H
