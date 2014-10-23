#ifndef BINGOT_H
#define BINGOT_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QVector>
#include "BlockChain.h"
#include "Block.h"
#include "Transaction.h"
#include "BlockChain.h"
#include "Miner.h"

class Bingot : public QObject
{
    Q_OBJECT

    QByteArray m_privateKey;
    QByteArray m_publicKey;
    QByteArray m_address;

    QHash<QByteArray, Transaction> m_suggestedTransactions;
    Block m_candidateBlock;
    BlockChain m_blockChain;

    QVector<Miner*> m_miners;

public:
    explicit Bingot(QObject *parent = 0);

    void initialize();

    const QByteArray address() const
    {
        return m_address;
    }

    const QByteArray privateKey() const
    {
        return m_privateKey;
    }

    const QByteArray publicKey() const
    {
        return m_publicKey;
    }

    void Transfer(const QByteArray &address, unsigned int amount);

    void startNewMiningRound();



signals:

public slots:
    void newBlockReceived(Block b);
    void newBlockSolved(Block b);

private:
    void generateECDSAKeyPair();
    void generateWalletAddress();

};

#endif // BINGOT_H
