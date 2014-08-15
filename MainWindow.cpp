#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "Bingot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Bingot b;
    b.initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}
