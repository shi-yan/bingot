#include "BlockChain.h"
#include <QCryptographicHash>
#include <QDebug>

BlockChain::BlockChain(QObject *parent) :
    QObject(parent)
{
}

int BlockChain::size()
{
    return m_chain.size();
}

QByteArray BlockChain::getLastBlockHash()
{
    return m_chain.last().first().getCacheBlockHash();
}

const QVector<Block> &BlockChain::at(int index)
{
    return m_chain.at(index);
}

bool BlockChain::add(const Block &b)
{
    //need to check if b is redundant.
    // need to verify each transaction in b is valid
    if ((b.getIndex() <= size()) && (b.getIndex() >= size()-2))
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

BlockChain::TransactionState BlockChain::getTransactionState(const Transaction &t, unsigned int &total)
{
    if (t.getAmount() == 0 || size() == 0)
    {
        return NOT_ENOUGH_MONEY;
    }
    else
    if (size() > 0)
    {
        total = 0;

        QByteArray preHash;
        for(int i = size() - 1; i>=0; ++i)
        {
            int blockIndex = -1;
            if (i == size() - 1)
            {
                preHash = m_chain[i].at(0).getPreHash();
                blockIndex = 0;
            }
            else
            {
                for(int e = 0; e< m_chain[i].size(); ++e)
                {
                    if (m_chain[i][e].getCacheBlockHash() == preHash)
                    {
                        preHash = m_chain[i][e].getPreHash();
                        blockIndex = e;
                        break;
                    }
                }
            }

            Q_ASSERT(blockIndex != -1);

            Block b = m_chain[i].at(blockIndex);

            {
                QHash<QByteArray, Transaction> transactionsInBlock = b.getTransactions();

                for(QHash<QByteArray, Transaction>::iterator iter = transactionsInBlock.begin(); iter != transactionsInBlock.end(); ++iter)
                {
                    if (iter.key() == t.getSignature())
                    {
                        if (iter.value() == t)
                        {
                            return EXISITING;
                        }
                    }
                    else if(iter.value().getToAddress() == t.getFromAddress())
                    {
                        total += iter.value().getAmount();
                    }
                }
            }

            qDebug() << "Total value for account" << t.getFromAddress() << total;

            if (total > t.getAmount())
            {
                return VALID;
            }
            else
            {
                return NOT_ENOUGH_MONEY;
            }
        }
    }

    return UNCHECKED;
}

unsigned int BlockChain::getAccountAmount(const QByteArray &toAddress)
{
    unsigned int total = 0;

    if (size() > 0)
    {
        QByteArray preHash;
        for(int i = size() - 1; i>=0; ++i)
        {
            int blockIndex = -1;
            if (i == size() - 1)
            {
                preHash = m_chain[i].at(0).getPreHash();
                blockIndex = 0;
            }
            else
            {
                for(int e = 0; e< m_chain[i].size(); ++e)
                {
                    if (m_chain[i][e].getCacheBlockHash() == preHash)
                    {
                        preHash = m_chain[i][e].getPreHash();
                        blockIndex = e;
                        break;
                    }
                }
            }

            Q_ASSERT(blockIndex != -1);

            Block b = m_chain[i].at(blockIndex);

            {
                QHash<QByteArray, Transaction> transactionsInBlock = b.getTransactions();

                for(QHash<QByteArray, Transaction>::iterator iter = transactionsInBlock.begin(); iter != transactionsInBlock.end(); ++iter)
                {
                    if(iter.value().getToAddress() == toAddress)
                    {
                        total += iter.value().getAmount();
                    }
                }
            }

            qDebug() << "Total value for account" << toAddress << total;
        }
    }

    return total;
}
