#ifndef MINER_H
#define MINER_H

#include <QThread>

class Miner : public QThread
{
    Q_OBJECT

public:
    Miner();

    void run();
};

#endif // MINER_H
