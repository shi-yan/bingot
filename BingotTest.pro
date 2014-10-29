#-------------------------------------------------
#
# Project created by QtCreator 2014-08-14T09:49:22
#
#-------------------------------------------------
QMAKE_MAC_SDK = macosx10.9

QT       += core gui network testlib


SOURCES += Transaction.cpp \
    BingotTest.cpp


HEADERS  += Transaction.h \
    BingotTest.h

macx: LIBS += -L$$PWD/../code/cryptopp562/ -lcryptopp

INCLUDEPATH += $$PWD/../code/cryptopp562
DEPENDPATH += $$PWD/../code/cryptopp562

macx: PRE_TARGETDEPS += $$PWD/../code/cryptopp562/libcryptopp.a
