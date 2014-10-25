#include <QApplication>
#include "Bingot.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "test";

    return a.exec();
}
