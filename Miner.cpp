#include "Miner.h"
#include "BigInt.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QElapsedTimer>

unsigned int Miner::countLeadingZeros(const QByteArray &hash)
{
    unsigned int result = 0;

    for(int i = hash.size() - 1; i >= 0; --i)
    {
        unsigned char byte = hash.data()[i];

        if (byte == 0x0)
        {
            result += 8;
            continue;
        }
        else
        {
            int n = 0;
            if (byte <= 0x0F) {n = n + 4; byte = byte << 4;}
            if (byte <= 0x3F) {n = n + 2; byte = byte << 2;}
            if (byte <= 0x7F) {n = n + 1;}
            result += n;
            break;
        }
    }

    return result;
}

Miner::Miner(const Block &blockToSolve, quint64 begin, quint64 end)
    :QThread(),
      m_blockToSolve(blockToSolve),
      m_begin(begin),
      m_end(end),
      m_target(0),
      m_solved(false),
      m_stop(false)
{
    m_target = Block::getTarget(m_blockToSolve.getIndex());
}

void Miner::run()
{
    qDebug() << "target" << m_target;
    QByteArray blockJson = m_blockToSolve.toJson();

    BigInt solution(m_begin);
    QElapsedTimer timer;
    timer.start();
    for(int i = m_begin; ((i < m_end) && !m_stop); ++i)
    {
        solution.increase();
        QByteArray data;
        data.push_back(solution.getData());
        data.push_back(blockJson);
        QCryptographicHash sha512(QCryptographicHash::Sha3_512);

        sha512.addData(data);

        QByteArray result = sha512.result();


        int zeroCount = Miner::countLeadingZeros(result);
        qDebug() << result.toHex() << "zero:" << zeroCount;
        if (zeroCount > m_target)
        {
            qDebug() << "========================= found ! =================== " << timer.elapsed()*0.001;
            m_blockToSolve.setSolution(solution);
            m_solved = true;
            emit newBlockSolved(m_blockToSolve);
            break;
        }
    }
}

bool Miner::hasSolved()
{
    return m_solved;
}

void Miner::forceStop()
{
    m_stop = true;
}
