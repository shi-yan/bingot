#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QByteArray>
#include <QString>
#include <QDateTime>

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

    friend bool operator==(const Transaction &a, const Transaction &b);

    QByteArray getJson();

    QByteArray getMessageJson();

    void signTransaction(const QByteArray &privateKey, const QByteArray &publicKey);

    bool verifySignature();

    const QByteArray getPublicKey() const;

    const QByteArray getSignature() const;

    TransactionType getType() const;

    const QByteArray getToAddress() const;
    const QByteArray getFromAddress() const;
    unsigned int getAmount() const;

    bool parseFromJsonObject(const QJsonObject &messageJson);

private:
    QByteArray m_toAddress;
    QByteArray m_fromAddress;
    unsigned int m_amount;
    TransactionType m_type;

    QByteArray m_signature;
    QByteArray m_publicKey;
    QDateTime m_timeStamp;
};

bool operator==(const Transaction &a, const Transaction &b);

#endif // TRANSACTION_H
