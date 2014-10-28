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
#include "NetworkEngine.h"

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

    NetworkEngine *m_networkEngine;

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

    void addSuggestedTransactions(Transaction &t);

signals:

public slots:
    void onNewBlockReceived(Block b);
    void onNewBlockSolved(Block b);
    void onNewTransaction(Transaction t);
    void onQueryBlock(int index);
    void onQueryBlockChainLength();
    void onReceivedBlockChainLength(int);

private:
    void generateECDSAKeyPair();
    QByteArray generateWalletAddress(const QByteArray publicKey = QByteArray());
    void queryForBlock(int index);
};

#endif // BINGOT_H
