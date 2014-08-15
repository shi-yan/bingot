#ifndef NETWORKENGINE_H
#define NETWORKENGINE_H

#include <QTcpServer>
#include <QList>
#include <QVector>
#include "Neighbor.h"
#include "SocketWorker.h"

class NetworkEngine : public QTcpServer
{
    Q_OBJECT
public:
    NetworkEngine();
    void incomingConnection(qintptr socketId);
    void initialize();
private:
    QList<Neighbor> neighborList;
    QVector<SocketWorker*> socketWorkerList;
};

#endif // NETWORKENGINE_H
