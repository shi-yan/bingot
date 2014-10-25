#ifndef MINER_H
#define MINER_H

#include <QThread>
#include "Block.h"

class Miner : public QThread
{
    Q_OBJECT

    Block m_blockToSolve;
    quint64 m_begin;
    quint64 m_end;
    int m_target;
    bool m_solved;
    bool m_stop;

public:
    Miner(const Block &blockToSolve, quint64 begin, quint64 end);
    bool hasSolved();
    void run();
    void forceStop();

    static unsigned int countLeadingZeros(const QByteArray &hash);

signals:
    void newBlockSolved(Block b);


};

#endif // MINER_H
