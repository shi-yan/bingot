#include "BigInt.h"
#include <QDebug>

BigInt::BigInt(quint64 val)
    :m_data()
{
    m_data.clear();
    m_data.push_back((char) 0);
    operator =(val);
}

BigInt::BigInt(const BigInt &in)
    :m_data(in.m_data)
{
}

void BigInt::operator=(const BigInt &in)
{
    m_data = in.m_data;
}

void BigInt::operator =(quint64 val)
{
    int index = 0;
    while (val > 0)
    {
        char byte = val & 0xff;
        if (index >= m_data.size())
        {
            m_data.push_back((char) 0);
        }
        m_data.data()[index++] = byte;
        val >>= 8;
    }
}

void BigInt::increase()
{
    for(int i = 0; i < m_data.size(); ++i)
    {
        if (((unsigned char*) m_data.data())[i] < (unsigned char) 0xff)
        {
            ((unsigned char*) m_data.data())[i]++;
            return;
        }
        else
        {
            ((unsigned char*) m_data.data())[i] = 0x0;
        }
    }
    m_data.push_back((unsigned char) 0x1);
}

const QByteArray BigInt::getData() const
{
    return m_data;
}

void BigInt::setData(const QByteArray &in)
{
    m_data.clear();
    m_data = in;
}

bool operator==(const BigInt &a, const BigInt &b)
{
    if (a.m_data.size() != b.m_data.size())
    {
        return false;
    }

    for(int i = 0; i < a.m_data.size(); ++i)
    {
        if (a.m_data[i] != b.m_data[i])
        {
            return false;
        }
    }

    return true;
}
