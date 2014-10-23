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

public:
    Miner(const Block &blockToSolve, quint64 begin, quint64 end);
    bool hasSolved();
    void run();


};

#endif // MINER_H
