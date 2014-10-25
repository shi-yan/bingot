#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <QObject>
#include <QVector>
#include "Block.h"

class BlockChain : public QObject
{
    Q_OBJECT

private:
    QVector<QVector<Block> > m_chain;

public:
    typedef enum
    {
        UNCHECKED,
        NOT_ENOUGH_MONEY,
        EXISITING,
        VALID
    } TransactionState;

    explicit BlockChain(QObject *parent = 0);

    int size();
    QByteArray getLastBlockHash();

    bool add(const Block &b);

    void prune();

    TransactionState getTransactionState(const Transaction &t, unsigned int &amount);

    unsigned int getAccountAmount(const QByteArray &toAddress);

    const QVector<Block> &at(int index);

signals:

public slots:

};

#endif // BLOCKCHAIN_H
