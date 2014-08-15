#include "NetworkEngine.h"

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
        SocketWorker *worker = new SocketWorker(m_taskQueue);
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
