#include "BlockChain.h"
#include <QCryptographicHash>

BlockChain::BlockChain(QObject *parent) :
    QObject(parent)
{
}

int BlockChain::size()
{
    return m_chain.size();
}

QByteArray BlockChain::getLastBlockHash()
{}

const QVector<Block> &BlockChain::at(int index)
{
    return m_chain.at(index);
}

bool BlockChain::add(const Block &b)
{
    if (b.getIndex() <= size())
    {
        //check preHash
        bool foundMatching = false;
        const QVector<Block> &previousBlocks = at(size() - 1);

        foreach(Block previousBlock, previousBlocks)
        {
            QByteArray messageJson = previousBlock.toMessageJson();
            QCryptographicHash sha512(QCryptographicHash::Sha3_512);

            sha512.addData(messageJson);

            if (sha512.result() == b.getPreHash())
            {
                foundMatching = true;
            }

        }

        if (foundMatching)
        {
            if (size() == b.getIndex())
            {
                m_chain.push_back(QVector<Block>());
                m_chain.last().push_back(b);
            }
            else
            {
                m_chain[b.getIndex()].push_back(b);
            }
        }
    }

    return false;
}
