#include "Transaction.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <cryptlib.h>
#include <osrng.h>
#include <eccrypto.h>
#include <oids.h>
#include <QDebug>

Transaction::Transaction()
    :m_toAddress(),
      m_fromAddress(),
      m_amount(0),
      m_type(REWARD),
      m_signature(),
      m_publicKey(),
      m_timeStamp()
{
}

Transaction::Transaction(const Transaction &in)
    :m_toAddress(in.m_toAddress),
      m_fromAddress(in.m_fromAddress),
      m_amount(in.m_amount),
      m_type(in.m_type),
      m_signature(in.m_signature),
      m_publicKey(in.m_publicKey),
      m_timeStamp(in.m_timeStamp)
{
}

Transaction::Transaction(const QByteArray &from, const QByteArray &to, unsigned int amount)
    :m_toAddress(to),
      m_fromAddress(from),
      m_amount(amount),
      m_type(NORMAL),
      m_signature(),
      m_publicKey(),
      m_timeStamp(QDateTime::currentDateTime())
{
}

Transaction::Transaction(const QByteArray &to)
    :m_toAddress(to),
      m_fromAddress(to),
      m_amount(25),
      m_type(REWARD),
      m_signature(),
      m_publicKey(),
      m_timeStamp(QDateTime::currentDateTime())
{
}

void Transaction::operator=(const Transaction &in)
{
    m_toAddress = in.m_toAddress;
    m_fromAddress = in.m_fromAddress;
    m_amount = in.m_amount;
    m_type = in.m_type;
    m_signature = in.m_signature;
    m_publicKey = in.m_publicKey;
    m_timeStamp = in.m_timeStamp;
}

QByteArray Transaction::getJson()
{
    QJsonObject obj;
    obj["to"] = QString::fromLocal8Bit(m_toAddress);
    obj["from"] = QString::fromLocal8Bit(m_fromAddress);
    obj["amount"] = (qint64) m_amount;
    obj["type"] = m_type;
    obj["time"] = QString::number( m_timeStamp.toMSecsSinceEpoch());

    QJsonDocument jdoc(obj);

    return jdoc.toJson();
}

void Transaction::signTransaction(const QByteArray &privateKeyData, const QByteArray &publicKey)
{
    qDebug () << "private key for signing:" << privateKeyData.toHex();
    CryptoPP::AutoSeededRandomPool autoSeededRandomPool;

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKey;
    privateKey.Initialize(autoSeededRandomPool, CryptoPP::ASN1::secp160r1());

    CryptoPP::Integer privateKeyInteger = 0;
    privateKeyInteger.Decode((byte*)privateKeyData.data(), (size_t)privateKeyData.size());
    privateKey.SetPrivateExponent(privateKeyInteger);

    bool result = privateKey.Validate( autoSeededRandomPool, 3 );
    if( !result )
    {
        qDebug() << "private key is not valid!";
        return;
    }

    QByteArray message = getJson();
    qDebug() << "transaction message:" << message;

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer(privateKey);

    result = signer.AccessKey().Validate( autoSeededRandomPool, 3 );
    if( !result )
    {
        qDebug() << "failed to create signer" << result;
        return;
    }

    // Determine maximum size, allocate a string with the maximum size
    size_t siglen = signer.MaxSignatureLength();
    QByteArray signature(siglen, 0x00);


    // Sign, and trim signature to actual size
    siglen = signer.SignMessage( autoSeededRandomPool, (const byte*)message.data(), message.size(), (byte*)signature.data() );

    m_signature = QByteArray(signature.data(), siglen);
    m_publicKey = publicKey;
}

QByteArray Transaction::getMessageJson()
{
    QJsonObject obj;
    obj["message"] = "Transaction";

    QByteArray cjson = getJson();
    QJsonParseError error;
    QJsonDocument cjdoc =QJsonDocument::fromJson(cjson, &error);

    obj["transaction"] = cjdoc.object();
    obj["signature"] = QString::fromLocal8Bit(m_signature.toBase64());
    obj["public_key"] = QString::fromLocal8Bit(m_publicKey.toBase64());

    QJsonDocument jdoc(obj);
    return jdoc.toJson();
}

bool Transaction::parseFromJsonObject(const QJsonObject &messageJson)
{
    QJsonObject cobj = messageJson["transaction"].toObject();
    m_toAddress =  cobj["to"].toString().toLocal8Bit();
    m_fromAddress = cobj["from"].toString().toLocal8Bit();
    m_amount = cobj["amount"].toInt();
    m_type = (TransactionType) cobj["type"].toInt();
    m_signature = QByteArray::fromBase64(messageJson["signature"].toString().toLocal8Bit());
    m_publicKey = QByteArray::fromBase64(messageJson["public_key"].toString().toLocal8Bit());
    m_timeStamp = QDateTime::fromMSecsSinceEpoch(cobj["time"].toString().toULongLong());
    return true;
}


bool Transaction::verifySignature()
{
    CryptoPP::AutoSeededRandomPool autoSeededRandomPool;

    QByteArray xData = m_publicKey.left(m_publicKey.size() / 2);
    QByteArray yData = m_publicKey.right(m_publicKey.size() / 2);

    qDebug() << xData.toHex() << yData.toHex();

    CryptoPP::ECP::Point point;

    point.x.Decode((byte*)xData.data(), (size_t)xData.size());
    point.y.Decode((byte*)yData.data(), (size_t)yData.size());
    point.identity = false;

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKey;

    publicKey.Initialize( CryptoPP::ASN1::secp160r1(), point );

    bool result = publicKey.Validate( autoSeededRandomPool, 3 );
    if( !result )
    {
        qDebug() << "public key is not valid!";
        return false;
    }

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier( publicKey );
    QByteArray message = getJson();

    result = verifier.VerifyMessage( (const byte*)message.data(), message.size(), (const byte*)m_signature.data(), m_signature.size() );
    if(result)
    {
        qDebug() << "Verified signature on message";
        return true;
    }
    else
    {
        qDebug() << "Failed to verify signature on message";
        return false;
    }
}

const QByteArray Transaction::getPublicKey() const
{
    return m_publicKey;
}

const QByteArray Transaction::getSignature() const
{
    return m_signature;
}

Transaction::TransactionType Transaction::getType() const
{
    return m_type;
}

const QByteArray Transaction::getToAddress() const
{
    return m_toAddress;
}

const QByteArray Transaction::getFromAddress() const
{
    return m_fromAddress;
}

unsigned int Transaction::getAmount() const
{
    return m_amount;
}

bool operator==(const Transaction &a, const Transaction &b)
{
    return (a.m_toAddress == b.m_toAddress) &&
       (a.m_fromAddress == b.m_fromAddress) &&
                 (a.m_amount == b.m_amount) &&
                     (a.m_type == b.m_type) &&
           (a.m_signature == b.m_signature) &&
           (a.m_publicKey == b.m_publicKey) &&
           (a.m_timeStamp == b.m_timeStamp);
}
