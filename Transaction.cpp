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
      m_type(REWARD)
{
}

Transaction::Transaction(const Transaction &in)
    :m_toAddress(in.m_toAddress),
      m_fromAddress(in.m_fromAddress),
      m_amount(in.m_amount),
      m_type(in.m_type)
{
}

Transaction::Transaction(const QByteArray &from, const QByteArray &to, unsigned int amount)
    :m_toAddress(to),
      m_fromAddress(from),
      m_amount(amount),
      m_type(NORMAL)
{
}

Transaction::Transaction(const QByteArray &to)
    :m_toAddress(to),
      m_fromAddress(to),
      m_amount(25),
      m_type(REWARD)
{
}

void Transaction::operator=(const Transaction &in)
{
    m_toAddress = in.m_toAddress;
    m_fromAddress = in.m_fromAddress;
    m_amount = in.m_amount;
    m_type = in.m_type;
}

QByteArray Transaction::getJson()
{
    QJsonObject obj;
    obj["to"] = QString::fromUtf8(m_toAddress.toBase64());
    obj["from"] = QString::fromUtf8(m_toAddress.toBase64());;
    obj["amount"] = (qint64) m_amount;
    obj["type"] = m_type;

    QJsonDocument jdoc(obj);

    return jdoc.toJson();
}

void Transaction::signTransaction(const QByteArray &privateKeyData)
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

    m_signature =QByteArray(signature.data(), siglen);
}

QByteArray Transaction::getMessageJson()
{
    QJsonObject obj;
    obj["message"] = "Transaction";

    QJsonObject cobj;
    cobj["to"] = QString::fromUtf8(m_toAddress.toBase64());
    cobj["from"] = QString::fromUtf8(m_toAddress.toBase64());;
    cobj["amount"] = (qint64) m_amount;
    cobj["type"] = m_type;

    obj["content"] = cobj;
    obj["signature"] = QString::fromUtf8(m_signature.toBase64());

    QJsonDocument jdoc(obj);
    return jdoc.toJson();
}

bool Transaction::verifySignature(const QByteArray &publicKeyData)
{
    CryptoPP::AutoSeededRandomPool autoSeededRandomPool;

    QByteArray xData = publicKeyData.left(publicKeyData.size() / 2);
    QByteArray yData = publicKeyData.right(publicKeyData.size() / 2);

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
