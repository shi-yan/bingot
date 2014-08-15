#include "NetworkTaskQueue.h"

NetworkTaskQueue::NetworkTaskQueue()
    :QObject(),
      m_taskQueue(),
      m_taskQueueMutex(),
      m_emptyQueueSemaphore()
{
}

void NetworkTaskQueue::pushTask(NetworkTask *task)
{
    m_taskQueueMutex.lock();
    m_taskQueue.push_back(task);
    m_taskQueueMutex.unlock();
    m_emptyQueueSemaphore.release();
}

NetworkTask* NetworkTaskQueue::popTask()
{
    NetworkTask *result = NULL;
    m_emptyQueueSemaphore.acquire();
    m_taskQueueMutex.lock();
    result = m_taskQueue.front();
    m_taskQueue.pop_front();
    m_taskQueueMutex.unlock();
    return result;
}
