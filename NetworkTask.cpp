#include "NetworkTask.h"

NetworkTask::NetworkTask()
    :QObject()
{
}

SendTask::SendTask()
    :NetworkTask()
{}

NetworkTask::TaskType SendTask::getType()
{
    return NetworkTask::SEND;
}

ReceiveTask::ReceiveTask(qintptr socketId)
    :NetworkTask(),
      m_socketId(socketId)
{}

NetworkTask::TaskType ReceiveTask::getType()
{
    return NetworkTask::RECEIVE;
}

qintptr ReceiveTask::getSocketId()
{
    return m_socketId;
}
