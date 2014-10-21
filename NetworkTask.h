#ifndef NETWORKTASK_H
#define NETWORKTASK_H

#include <QObject>
#include <QHostAddress>

class NetworkTask : public QObject
{

public:
    typedef enum
    {
        RECEIVE,
        SEND
    } TaskType;

    NetworkTask();

    virtual TaskType getType() = 0;
};


class SendTask : public NetworkTask
{
public:
    SendTask(const QByteArray &message, const QHostAddress &ip, const unsigned short port);

    TaskType getType();

    const QByteArray &getMessage();
    const QHostAddress &getIp();
    unsigned short getPort();

private:
    QByteArray m_message;
    QHostAddress m_ip;
    unsigned short m_port;
};

class ReceiveTask : public NetworkTask
{
public:
    ReceiveTask(qintptr socketId);

    TaskType getType();

    qintptr getSocketId();

private:
    qintptr m_socketId;
};

#endif // NETWORKTASK_H
