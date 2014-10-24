#include "Miner.h"
#include <QCryptographicHash>

Miner::Miner(const Block &blockToSolve, quint64 begin, quint64 end)
    :QThread(),
      m_blockToSolve(blockToSolve),
      m_begin(begin),
      m_end(end),
      m_target(0),
      m_solved(false)
{
    m_target = Block::getTarget(m_blockToSolve.getIndex());
}

void Miner::run()
{
    QByteArray blockJson = m_blockToSolve.toJson();

    for(int i = m_begin; i< m_end;++i)
    {
        QByteArray data;
        data.push_back(i);
        data.push_back(blockJson);
        QCryptographicHash sha512(QCryptographicHash::Sha3_512);

        sha512.addData(data);

        QByteArray result = sha512.result().toHex();

        int zeroCount = 0;
        while(result.at(zeroCount) == '0')
        {
            zeroCount++;
        }

        if (zeroCount > m_target)
        {
            m_blockToSolve.setSolution(i);
            m_solved = true;
            break;
        }
    }
    exec();
}

bool Miner::hasSolved()
{
    return m_solved;
}
