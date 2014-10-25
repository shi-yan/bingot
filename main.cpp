#include <QApplication>
#include "Bingot.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "BigInt.h"
#include "Miner.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "test";

    Bingot *bingot = new Bingot();
    bingot->initialize();

    qDebug() << "address:" << bingot->address();
    qDebug() << "privatekey:" << bingot->privateKey();
    qDebug() << "publicKey:" << bingot->publicKey();

    Transaction t(bingot->address(), QString("random_address").toLocal8Bit(), 53);
    t.signTransaction(bingot->privateKey(), bingot->publicKey());
    if (t.verifySignature())
    {
        qDebug() << "signature can be verified";
    }

    qDebug() << "from" << t.getFromAddress();
    qDebug() << "to" << t.getToAddress();

    QByteArray transJson = t.getMessageJson();

    Transaction t2;

    t2.parseFromJsonObject(QJsonDocument::fromJson(transJson).object());

    qDebug() << "t1 == t2?" << (t == t2);

    qDebug() << "verify t2 signature" << t2.verifySignature();

    qDebug() << "t2" << t2.getMessageJson();

    BigInt a1(0x1FF);
    qDebug() << "created";
    qDebug() << a1.getData().toHex() << Miner::countLeadingZeros(a1.getData());
    a1.increase();
    qDebug() << a1.getData().toHex() << Miner::countLeadingZeros(a1.getData());
    a1.increase();
    qDebug() <<a1.getData().toHex()<< Miner::countLeadingZeros(a1.getData());
    a1.increase();
    qDebug() <<a1.getData().toHex()<< Miner::countLeadingZeros(a1.getData());

    QHash<QByteArray, Transaction> h;
    h.insert(t.getSignature(), t);
    Block b(1, h, QString("random_prehash").toLocal8Bit());

    qDebug() << b.getCacheBlockHash();
    qDebug() << b.toJson();
    qDebug() << b.toMessageJson();

    Block b2;
    b2.parseFromQJsonObject(QJsonDocument::fromJson( b.toMessageJson()).object());

    qDebug() << "b2:"<< b2.toMessageJson();

    Miner *m_miner = new Miner(b2, 0, 20000000000000);
    m_miner->moveToThread(m_miner);
    m_miner->start();

    return a.exec();
}
