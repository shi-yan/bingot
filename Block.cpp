#include "Block.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Block::Block(int index)
    :m_index(index),
      m_preHash(),
      m_transactions(),
      m_solution(0)
{
}

QByteArray Block::toJson() const
{
    QJsonObject jobj;

    jobj["prehash"] = QString::fromUtf8(m_preHash.toBase64());

    QJsonArray jarr;

    foreach(Transaction t, m_transactions)
    {
        QJsonObject tobj = QJsonDocument::fromJson( t.getMessageJson()).object();
        jarr.append(tobj);
    }

    jobj["transactions"] = jarr;

    jobj["solution"] = m_solution;

    QJsonDocument jdoc(jobj);

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

void Block::setSolution(quint64 solution)
{
    m_solution = solution;
}

bool Block::isValid()
{

}
