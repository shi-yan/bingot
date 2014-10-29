#ifndef BINGOTTEST_H
#define BINGOTTEST_H

#include <QtTest/QtTest>

class BingotTest : public QObject
{
    Q_OBJECT

private slots:
    void toUpper();
};

#endif // BINGOTTEST_H
