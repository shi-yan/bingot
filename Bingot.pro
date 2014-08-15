#-------------------------------------------------
#
# Project created by QtCreator 2014-08-14T09:49:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bingot
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Bingot.cpp \
    BlockChain.cpp \
    Block.cpp \
    Transaction.cpp \
    Miner.cpp \
    NetworkEngine.cpp \
    SocketWorker.cpp \
    NetworkTaskQueue.cpp \
    Neighbor.cpp

HEADERS  += MainWindow.h \
    Bingot.h \
    BlockChain.h \
    Block.h \
    Transaction.h \
    Miner.h \
    NetworkEngine.h \
    Global.h \
    SocketWorker.h \
    NetworkTaskQueue.h \
    Neighbor.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../cryptopp

LIBS += -L../cryptopp \
        -lcryptopp

