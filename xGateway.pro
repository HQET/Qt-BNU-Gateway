#-------------------------------------------------
#
# Project created by QtCreator 2016-09-02T19:25:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xGateway
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RC_ICONS = wifi.ico
ICON = wifi.icns

DISTFILES += \
    wifi.ico \
    wifi.icns
