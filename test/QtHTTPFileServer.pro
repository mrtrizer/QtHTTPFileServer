#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T19:57:12
#
#-------------------------------------------------

QT       += core gui network

include("../src/qtfileserver.pri")

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtHTTPFileServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \

HEADERS  += mainwindow.h \

FORMS    += mainwindow.ui
