#-------------------------------------------------
#
# Project created by QtCreator 2014-12-03T18:38:34
#
#-------------------------------------------------

QT       += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WakxySniffer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sniffer.cpp \
    log.cpp \
    packeteditor.cpp \
    utils.cpp \
    packetzoomdialog.cpp

HEADERS  += mainwindow.h \
    define.h \
    sniffer.h \
    log.h \
    packeteditor.h \
    utils.h \
    packetzoomdialog.h

FORMS    += mainwindow.ui \
    packetzoomdialog.ui
