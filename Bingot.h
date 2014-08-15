#ifndef BINGOT_H
#define BINGOT_H

#include <QObject>
#include <QByteArray>

#include "BlockChain.h"
#include "Block.h"
#include "Transaction.h"

class Bingot : public QObject
{
    Q_OBJECT

    QByteArray m_privateKey;
    QByteArray m_publicKey;

    QByteArray m_address;

public:
    explicit Bingot(QObject *parent = 0);

    void initialize();

signals:

public slots:

private:
    void generateECDSAKeyPair();
    void generateWalletAddress();

};

#endif // BINGOT_H
