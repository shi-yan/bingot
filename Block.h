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

    quint64 m_solution;


public:
    explicit Block();


};

#endif // BLOCK_H
