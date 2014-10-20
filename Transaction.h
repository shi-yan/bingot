#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QByteArray>
#include <QString>

class Transaction
{
public:
    typedef enum
    {
        NORMAL,
        REWARD
    } TransactionType;

    Transaction();
    Transaction(const Transaction &in);
    Transaction(const QByteArray &from, const QByteArray &to, unsigned int amount);
    Transaction(const QByteArray &to);

    void operator=(const Transaction &in);

    QByteArray getJson();

    QByteArray getMessageJson();

    void signTransaction(const QByteArray &privateKey);

    bool verifySignature(const QByteArray &publicKey);

private:
    QByteArray m_toAddress;
    QByteArray m_fromAddress;
    unsigned int m_amount;
    TransactionType m_type;

    QByteArray m_signature;
};

#endif // TRANSACTION_H
