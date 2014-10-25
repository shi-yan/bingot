#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include "Transaction.h"
#include "BigInt.h"

class Block
{
    QByteArray m_preHash;

    QHash<QByteArray, Transaction> m_transactions;

    BigInt m_solution;

    int m_index;

    QByteArray m_cachedBlockHash;

public:
    Block(int index = 0);
    Block(int index, const QHash<QByteArray, Transaction> &transactions, const QByteArray &preHash);

    QByteArray toJson() const;
    QByteArray toMessageJson() const;

    void addTransaction(const Transaction &t);
    void removeTransaction(const Transaction &t);
    int getIndex() const;
    void setSolution(BigInt solution);
    bool isValid();

    const QByteArray &getPreHash() const;
    const QByteArray &getCacheBlockHash();

    const QHash<QByteArray, Transaction> &getTransactions() const;

    void refreshBlockHash();

    static unsigned int getTarget(int index);

    bool parseFromQJsonObject(const QJsonObject &obj);
};

#endif // BLOCK_H
