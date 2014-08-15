#ifndef NETWORKTASK_H
#define NETWORKTASK_H

#include <QObject>

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
    SendTask();

    TaskType getType();
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
