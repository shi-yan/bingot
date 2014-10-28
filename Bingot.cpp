#include "Bingot.h"
#include <cryptlib.h>
#include <osrng.h>
#include <eccrypto.h>
#include <oids.h>
#include <QDebug>

#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>

const unsigned int protocol_version = 1;

Bingot::Bingot(QObject *parent) :
    QObject(parent),
    m_candidateBlock(0),
    m_networkEngine()
{
}

void Bingot::initialize()
{
    m_networkEngine = new NetworkEngine();
    m_networkEngine->listen(QHostAddress::Any, 42231);
    generateECDSAKeyPair();
    m_address = generateWalletAddress();
}

void Bingot::generateECDSAKeyPair()
{
    CryptoPP::AutoSeededRandomPool autoSeededRandomPool;

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKey;

    privateKey.Initialize(autoSeededRandomPool, CryptoPP::ASN1::secp160r1());

    bool result = privateKey.Validate( autoSeededRandomPool, 3 );
    if( !result )
    {
        qDebug() << "private key is not valid!";
        return;
    }

    const CryptoPP::Integer &privateKeyInteger = privateKey.GetPrivateExponent();

    QByteArray privateKeyByteArray((int)privateKeyInteger.ByteCount(), 0);
    privateKeyInteger.Encode((byte*)privateKeyByteArray.data(), privateKeyInteger.ByteCount());
    m_privateKey = privateKeyByteArray;

    qDebug() << "private key:" << m_privateKey.size() << m_privateKey.toHex();
    // Generating matching public key
    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKey;
    privateKey.MakePublicKey(publicKey);

    result = publicKey.Validate( autoSeededRandomPool, 3 );
    if( !result )
    {
        qDebug() << "public key is not valid!";
        return;
    }

    const CryptoPP::ECP::Point& point = publicKey.GetPublicElement();

    QByteArray publicKeyXByteArray((int)point.x.ByteCount(), 0);
    point.x.Encode((byte*)publicKeyXByteArray.data(), point.x.ByteCount());

    QByteArray publicKeyYByteArray((int)point.y.ByteCount(), 0);
    point.y.Encode((byte*)publicKeyYByteArray.data(), point.y.ByteCount());

    m_publicKey.clear();
    m_publicKey.append(publicKeyXByteArray);
    m_publicKey.append(publicKeyYByteArray);

    qDebug() << "public key:" << m_publicKey.size() << m_publicKey.toHex();
}

QByteArray Bingot::generateWalletAddress(QByteArray publicKey)
{
    QCryptographicHash hasher1(QCryptographicHash::Sha3_512);

    if (publicKey.isEmpty())
    {
        hasher1.addData(m_publicKey);
    }
    else
    {
        hasher1.addData(publicKey);
    }

    QByteArray a1 = hasher1.result();
    a1.prepend((const char*)&protocol_version, sizeof(protocol_version));

    qDebug() << "hash result:" << a1.toHex();

    QCryptographicHash hasher2(QCryptographicHash::Sha256);

    hasher2.addData(a1);
    QByteArray a2 = hasher2.result();

    qDebug() << "hash result:" << a2.toHex();


    QCryptographicHash hasher3(QCryptographicHash::Sha3_256);

    hasher3.addData(a2);

    QByteArray a3 = hasher3.result();

    qDebug() << "cehcksum:" << a3.toHex();

    a1.append(a3.mid(0,4));

    qDebug() << "with checksum" << a1.toHex();

    return a1.toBase64();
}

void Bingot::Transfer(const QByteArray &toAddress, unsigned int amount)
{
    if (toAddress == m_address)
    {
        return;
    }

    int total = m_blockChain.getAccountAmount(m_address);

    foreach(const Transaction &t, m_suggestedTransactions)
    {
        if ((t.getType() != Transaction::REWARD) && (t.getFromAddress() == m_address))
        {
            total -= t.getAmount();
        }
    }

    const QHash<QByteArray, Transaction> &candidateTransactions = m_candidateBlock.getTransactions();

    for(QHash<QByteArray, Transaction>::const_iterator iter = candidateTransactions.begin(); iter != candidateTransactions.end(); ++iter)
    {
        if ((iter->getType() != Transaction::REWARD && (iter->getFromAddress() == m_address)))
        {
            total -= iter->getAmount();
        }
    }

    if (amount <= total)
    {
        Transaction t(address(), toAddress, amount);
        t.signTransaction(privateKey(), publicKey());
        m_suggestedTransactions.insert(t.getSignature(), t);
        m_networkEngine->sendMessage(t.getMessageJson());
    }
}

void Bingot::startNewMiningRound()
{
    onQueryBlockChainLength();

    Block newCandidateBlock(m_blockChain.size(), m_suggestedTransactions, m_blockChain.getLastBlockHash());

    m_candidateBlock = newCandidateBlock;
    m_candidateBlock.addTransaction(Transaction(address()));

    m_suggestedTransactions.clear();

    const unsigned int workerThreadCount = QThread::idealThreadCount();

    quint64 value = Q_UINT64_C(932838457459459);
    quint64 chunk = value/workerThreadCount;

    foreach(Miner *miner, m_miners)
    {
        miner->forceStop();
        miner->exit();
        miner->deleteLater();
    }

    m_miners.clear();

    for(unsigned int i = 0; i < workerThreadCount; ++i)
    {
        Miner *miner = new Miner(m_candidateBlock, chunk*i, chunk*(1+i));
        connect(miner,SIGNAL(newBlockSolved(Block)),this,SLOT(onNewBlockSolved(Block)));
        miner->moveToThread(miner);
        m_miners.push_back(miner);
        miner->start();
    }
}

void Bingot::onNewBlockReceived(Block b)
{
    if (b.isValid())
    {
        if (b.getIndex() == m_blockChain.size())
        {
           if( m_blockChain.add(b))
           {
                //recycle m_candidateBlock to m_suggestedTransactions;
               const QHash<QByteArray, Transaction> &transactions = m_candidateBlock.getTransactions();

                for(QHash<QByteArray, Transaction>::const_iterator t = transactions.begin(); t != transactions.end(); ++t)
                {
                    m_suggestedTransactions.insert(t.key(), t.value());
                }

                //remove m_suggestedTransactions that are in blockChain already

                QVector<Transaction> keepTransactions;

                foreach(const Transaction &t, m_suggestedTransactions)
                {
                    unsigned int total = 0;
                    BlockChain::TransactionState ts = m_blockChain.getTransactionState(t, total);

                    if (ts == BlockChain::VALID)
                    {
                        foreach(const Transaction &tr, m_suggestedTransactions)
                        {
                            if ((tr.getType() != Transaction::REWARD) && (tr.getFromAddress() == m_address))
                            {
                                total -= tr.getAmount();
                            }
                        }

                        const QHash<QByteArray, Transaction> &candidateTransactions = m_candidateBlock.getTransactions();

                        for(QHash<QByteArray, Transaction>::const_iterator iter = candidateTransactions.begin(); iter != candidateTransactions.end(); ++iter)
                        {
                            if ((iter->getType() != Transaction::REWARD && (iter->getFromAddress() == m_address)))
                            {
                                total -= iter->getAmount();
                            }
                        }

                        if (total > t.getAmount())
                        {
                            //accept;
                            keepTransactions.push_back(t);
                        }

                    }
                }

                m_suggestedTransactions.clear();

                foreach(Transaction t, keepTransactions)
                {
                   m_suggestedTransactions.insert(t.getSignature(), t);
                }

                m_networkEngine->sendMessage(b.toMessageJson());

                startNewMiningRound();
           }

        }
        else if(b.getIndex() < m_blockChain.size())
        {
            m_blockChain.add(b);
        }
        else if(b.getIndex() > m_blockChain.size())
        {
            queryForBlock(m_blockChain.size());
        }
    }
}

void Bingot::onQueryBlock(int index)
{
    if ((index >= 0) && (m_blockChain.size() > index))
    {
        QVector<Block> blocks = m_blockChain.at(index);
        m_networkEngine->sendMessage(blocks[0].toMessageJson());
    }
}

void Bingot::onReceivedBlockChainLength(int length)
{
    if (length > m_blockChain.size())
    {
        queryForBlock(m_blockChain.size());
    }
}

void Bingot::onQueryBlockChainLength()
{
    QJsonObject jobj;
    jobj["length"] = m_blockChain.size();
    jobj["message"] = "BlockChainLength";

    QJsonDocument jdoc(jobj);
    m_networkEngine->sendMessage(jdoc.toJson());
}

void Bingot::onNewBlockSolved(Block b)
{
    if((b.getIndex() == m_blockChain.size()) && m_blockChain.add(b))
    {
        m_networkEngine->sendMessage(b.toMessageJson());
    }
    startNewMiningRound();
}

void Bingot::onNewTransaction(Transaction t)
{
    if (t.getType() == Transaction::REWARD)
    {
        return; //reward transaction can only be part of solved block.
    }

    if (t.getToAddress() == t.getFromAddress())
    {
        return; //can't self transfer;
    }

    if ( generateWalletAddress(t.getPublicKey()) != t.getFromAddress())
    {
        return; //because from address and public key don't match
    }

    if (!t.verifySignature())
    {
        return;
    }

    //check transaction uniqueness,
    unsigned int total = 0;
    BlockChain::TransactionState ts = m_blockChain.getTransactionState(t, total);

    if (ts == BlockChain::EXISITING || ts == BlockChain::NOT_ENOUGH_MONEY)
    {
        return;
    }

    if (ts == BlockChain::VALID)
    {
        foreach(const Transaction &tr, m_suggestedTransactions)
        {
            if (tr.getSignature() == t.getSignature())
            {
                return;
            }
            if ((tr.getType() != Transaction::REWARD) && (tr.getFromAddress() == m_address))
            {
                total -= tr.getAmount();
            }
        }

        const QHash<QByteArray, Transaction> &candidateTransactions = m_candidateBlock.getTransactions();

        for(QHash<QByteArray, Transaction>::const_iterator iter = candidateTransactions.begin(); iter != candidateTransactions.end(); ++iter)
        {
            if (iter->getSignature() == t.getSignature())
            {
                return;
            }
            if ((iter->getType() != Transaction::REWARD && (iter->getFromAddress() == m_address)))
            {
                total -= iter->getAmount();
            }
        }

        if (total > t.getAmount())
        {
            //accept;
            m_suggestedTransactions.insert(t.getSignature(), t);
            //need to figure out a way to avoid sending this message to where it is from
            m_networkEngine->sendMessage(t.getMessageJson());
        }

    }
}

void Bingot::queryForBlock(int index)
{
    QJsonObject jobj;
    jobj["message"] = "QueryBlock";
    jobj["index"] = index;

    QJsonDocument jdoc(jobj);
    m_networkEngine->sendMessage(jdoc.toJson());
}
