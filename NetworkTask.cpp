#include "NetworkTask.h"

NetworkTask::NetworkTask()
    :QObject()
{
}

SendTask::SendTask(const QByteArray &message, const QHostAddress &ip, const unsigned short port)
    :NetworkTask(),
      m_message(message),
      m_ip(ip),
      m_port(port)
{
}

NetworkTask::TaskType SendTask::getType()
{
    return NetworkTask::SEND;
}

const QByteArray &SendTask::getMessage()
{
    return m_message;
}

const QHostAddress &SendTask::getIp()
{
    return m_ip;
}

unsigned short SendTask::getPort()
{
    return m_port;
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
