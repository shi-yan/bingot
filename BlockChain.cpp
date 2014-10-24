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
    //need to check redundancy.
    if (b.getIndex() <= size())
    {
        //check preHash
        bool foundMatching = false;
        const QVector<Block> &previousBlocks = at(size() - 1);

        foreach(Block previousBlock, previousBlocks)
        {
            if (previousBlock.getCacheBlockHash() == b.getPreHash())
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

void BlockChain::prune()
{
    for(int i = size() - 2;  i>= 0; --i)
    {
        if (m_chain[i].size() > 1)
        {
            QHash<QByteArray, int> mostPopularPreHash;

            foreach(Block b, m_chain[i+1])
            {
                if (mostPopularPreHash.contains(b.getPreHash()))
                {
                    mostPopularPreHash[b.getPreHash()] = 1;
                }
                else
                {
                    mostPopularPreHash[b.getPreHash()] ++;
                }
            }

            int largestPreHashCount = -1;
            QByteArray mostPopularPreHashArray;
            bool isMostPopularPreHashUnique = false;

            for(QHash<QByteArray, int>::iterator hashCountPair; hashCountPair!= mostPopularPreHash.end(); ++hashCountPair)
            {
                if (hashCountPair.value() > largestPreHashCount)
                {
                    largestPreHashCount = hashCountPair.value();
                    isMostPopularPreHashUnique = true;
                    mostPopularPreHashArray = hashCountPair.key();
                }
                else if(hashCountPair.value() == largestPreHashCount)
                {
                    isMostPopularPreHashUnique = false;
                }
            }

            if (!isMostPopularPreHashUnique)
            {
                continue;
            }

            Block left;

            foreach(Block b, m_chain[i])
            {
                if (b.getCacheBlockHash() == mostPopularPreHashArray)
                {
                    left = b;
                }
            }

            m_chain[i].clear();
            m_chain[i].push_back(left);
        }
    }
}
