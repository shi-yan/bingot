#ifndef SOCKETWORKER_H
#define SOCKETWORKER_H

#include <QThread>
#include <QTcpSocket>
#include <QByteArray>
#include "NetworkTaskQueue.h"
#include "Transaction.h"
#include "Block.h"

class NetworkEngine;
class SocketWorker : public QThread
{
    Q_OBJECT

public:
    SocketWorker(NetworkTaskQueue *taskQueue, NetworkEngine *networkEngine);
    void stop();

private:
    NetworkEngine *m_networkEngine;
    NetworkTaskQueue *m_taskQueue;
    bool m_hasTaskFinished;
    bool m_stop;
    QTcpSocket *m_socket;
    int m_incommingMessageSize;
    QByteArray m_tempDataBuffer;

    void waitForTaskFinish();
    void receiveMessage(ReceiveTask *task);
    void sendMessage(SendTask *task);

    void handleIncommingMessage(QByteArray json);

protected:
    void run();

signals:
    void newTransaction(Transaction t);
    void newBlock(Block b);
    void queryBlock(int index);
    void queryBlockChainLength();
    void getBlockChainLength(int);

private slots:
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError);
    void onSocketConnected();
    void onSocketDisconnected();
};

#endif // SOCKETWORKER_H
