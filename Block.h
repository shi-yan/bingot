#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QByteArray>

class Block : public QObject
{
    Q_OBJECT

    QByteArray m_preHash;

public:
    explicit Block(QObject *parent = 0);

signals:

public slots:

};

#endif // BLOCK_H
