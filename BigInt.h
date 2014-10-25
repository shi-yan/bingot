#ifndef BIGINT_H
#define BIGINT_H

#include <QByteArray>

class BigInt
{
public:
    BigInt(quint64 val = 0);
    BigInt(const BigInt &in);

    void operator=(const BigInt &in);
    void operator=(quint64 in);
    friend bool operator==(const BigInt &a, const BigInt &b);
    void increase();
    const QByteArray getData() const;
    void setData(const QByteArray &in);

private:
    QByteArray m_data;
};

bool operator==(const BigInt &a, const BigInt &b);

#endif // BIGINT_H
