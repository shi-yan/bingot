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
    explicit BlockChain(QObject *parent = 0);


    int size();
    QByteArray getLastBlockHash();
signals:

public slots:

};

#endif // BLOCKCHAIN_H
