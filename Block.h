#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include "Transaction.h"

class Block
{
    QByteArray m_preHash;

    QHash<QByteArray, Transaction> m_transactions;

    //I'm not sure if 64bit integer is enough
    quint64 m_solution;

    int m_index;

public:
    Block(int index = 0);
    Block(int index, const QHash<QByteArray, Transaction> &transactions, const QByteArray &preHash);

    QByteArray toJson() const;
    QByteArray toMessageJson() const;

    void addTransaction(const Transaction &t);
    void removeTransaction(const Transaction &t);
    int getIndex() const;
    void setSolution(quint64 solution);
    bool isValid();

    const QByteArray &getPreHash() const;

    static unsigned int getTarget(int index);
};

#endif // BLOCK_H
