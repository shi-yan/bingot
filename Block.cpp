#include "Block.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include "Miner.h"

unsigned int Block::getTarget(int index)
{
    return index / 1024 + 20;

    //155
    //500
    //2.49
    //42
    //429

}

Block::Block(int index)
    :m_index(index),
      m_preHash(),
      m_transactions(),
      m_solution(0)
{
}

Block::Block(int index, const QHash<QByteArray, Transaction> &transactions, const QByteArray &preHash)
    :m_index(index),
      m_transactions(transactions),
      m_preHash(preHash),
      m_solution(0)
{
}

QByteArray Block::toJson() const
{
    QJsonObject jobj;

    jobj["prehash"] = QString::fromLocal8Bit(m_preHash.toBase64());
    jobj["index"] = m_index;

    QJsonArray jarr;

    foreach(Transaction t, m_transactions)
    {
        QJsonObject tobj = QJsonDocument::fromJson( t.getMessageJson()).object();
        jarr.append(tobj);
    }

    jobj["transactions"] = jarr;

    QByteArray solutionArray;
    solutionArray.append((char*)&m_solution, sizeof(m_solution));

    QJsonDocument jdoc(jobj);

    return jdoc.toJson();
}

QByteArray Block::toMessageJson() const
{
    QJsonObject jobj;

    jobj["prehash"] = QString::fromLocal8Bit(m_preHash.toBase64());
    jobj["index"] = m_index;

    QJsonArray jarr;

    foreach(Transaction t, m_transactions)
    {
        QJsonObject tobj = QJsonDocument::fromJson( t.getMessageJson()).object();
        jarr.append(tobj);
    }

    jobj["transactions"] = jarr;

    QJsonObject messageJsonObj;

    messageJsonObj["message"] = "Block";
    messageJsonObj["block"] = jobj;
    messageJsonObj["solution"] = QString::fromLocal8Bit(m_solution.getData().toBase64());

    QJsonDocument jdoc(messageJsonObj);

    return jdoc.toJson();
}

void Block::addTransaction(const Transaction &t)
{
    m_transactions.insert(t.getSignature(), t);
}

void Block::removeTransaction(const Transaction &t)
{
    m_transactions.remove(t.getSignature());
}

int Block::getIndex() const
{
    return m_index;
}

void Block::setSolution(BigInt solution)
{
    m_solution = solution;
}

bool Block::isValid()
{
    //verify solution
    QByteArray blockJson = toJson();

    QByteArray data;
    data.push_back(m_solution.getData());
    data.push_back(blockJson);
    QCryptographicHash sha512(QCryptographicHash::Sha3_512);

    sha512.addData(data);

    QByteArray result = sha512.result();

    int zeroCount = Miner::countLeadingZeros(result);

    unsigned int target = Block::getTarget(m_index);

    if (zeroCount < target)
    {
        return false;
    }

    //check if there is only one reward transaction;
    int rewardTransactionCount = 0;
    foreach(Transaction t, m_transactions)
    {
        if (t.getType() == Transaction::REWARD)
        {
            ++ rewardTransactionCount;
        }
    }

    if (rewardTransactionCount != 1)
    {
        return false;
    }
}

const QByteArray &Block::getPreHash() const
{
    return m_preHash;
}

void Block::refreshBlockHash()
{
    QByteArray messageJson = toMessageJson();
    QCryptographicHash sha512(QCryptographicHash::Sha3_512);

    sha512.addData(messageJson);

    m_cachedBlockHash = sha512.result();
}

const QByteArray &Block::getCacheBlockHash()
{
    if (m_cachedBlockHash.isEmpty())
    {
        refreshBlockHash();
    }

    return m_cachedBlockHash;
}

const QHash<QByteArray, Transaction> &Block::getTransactions() const
{
    return m_transactions;
}

bool Block::parseFromQJsonObject(const QJsonObject &obj)
{
    QJsonObject jobj = obj["block"].toObject();

    m_preHash = QByteArray::fromBase64(jobj["prehash"].toString().toLocal8Bit());
    m_index = jobj["index"].toInt();

    QJsonArray jarr = jobj["transactions"].toArray();

    foreach(QJsonValue t, jarr)
    {
        QJsonObject tobj = t.toObject();
        Transaction transaction;
        transaction.parseFromJsonObject(tobj);
        m_transactions.insert(transaction.getSignature(), transaction);
    }

    QByteArray solution = QByteArray::fromBase64(obj["solution"].toString().toLocal8Bit());

    m_solution.setData(solution);

    return true;
}
