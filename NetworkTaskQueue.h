#ifndef NETWORKTASKQUEUE_H
#define NETWORKTASKQUEUE_H

#include <QQueue>
#include <QSemaphore>
#include <QMutex>

#include "NetworkTask.h"

class NetworkTaskQueue : public QObject
{
    Q_OBJECT
public:
    NetworkTaskQueue();

    void pushTask(NetworkTask *task);
    NetworkTask *popTask();

private:
    QQueue<NetworkTask *> m_taskQueue;
    QMutex m_taskQueueMutex;
    QSemaphore m_emptyQueueSemaphore;
};

#endif // NETWORKTASKQUEUE_H
