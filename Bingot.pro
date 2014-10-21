#-------------------------------------------------
#
# Project created by QtCreator 2014-08-14T09:49:22
#
#-------------------------------------------------
QMAKE_MAC_SDK = macosx10.9

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
    Neighbor.cpp \
    NetworkTask.cpp \
    NetworkTaskQueue.cpp

HEADERS  += MainWindow.h \
    Bingot.h \
    BlockChain.h \
    Block.h \
    Transaction.h \
    Miner.h \
    NetworkEngine.h \
    Global.h \
    SocketWorker.h \
    Neighbor.h \
    NetworkTask.h \
    NetworkTaskQueue.h

FORMS    += mainwindow.ui



macx: LIBS += -L$$PWD/../code/cryptopp562/ -lcryptopp

INCLUDEPATH += $$PWD/../code/cryptopp562
DEPENDPATH += $$PWD/../code/cryptopp562

macx: PRE_TARGETDEPS += $$PWD/../code/cryptopp562/libcryptopp.a
