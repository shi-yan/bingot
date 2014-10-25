#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "Bingot.h"
#include "Transaction.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Bingot b;
    b.initialize();

    Transaction t(b.address());
    t.signTransaction(b.privateKey(), b.publicKey());

    qDebug() << t.getMessageJson();

    qDebug() << t.verifySignature();

}

MainWindow::~MainWindow()
{
    delete ui;
}
