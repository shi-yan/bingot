#include "BlockChain.h"

BlockChain::BlockChain(QObject *parent) :
    QObject(parent)
{
}

int BlockChain::size()
{
    return 1;
}

QByteArray BlockChain::getLastBlockHash()
{}


bool BlockChain::add(const Block &b)
{

}
