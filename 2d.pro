#-------------------------------------------------
#
# Project created by QtCreator 2016-07-19T18:44:53
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2d
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    comchatter.cpp \
    logger.cpp

HEADERS  += mainwindow.h \
    comchatter.h \
    logger.h

FORMS    += \
    mainwindow.ui

RC_FILE = scriber.rc
