#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <QObject>

class BlockChain : public QObject
{
    Q_OBJECT
public:
    explicit BlockChain(QObject *parent = 0);


    int size();
    QByteArray getLastBlockHash();
signals:

public slots:

};

#endif // BLOCKCHAIN_H
