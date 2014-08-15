#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QObject>

class Transaction : public QObject
{
    Q_OBJECT
public:
    explicit Transaction(QObject *parent = 0);

signals:

public slots:

};

#endif // TRANSACTION_H
