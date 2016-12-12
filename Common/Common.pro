#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:15:54
#
#-------------------------------------------------

QT       -= gui

TARGET = Common
TEMPLATE = lib
CONFIG += staticlib

SOURCES += common.cpp

HEADERS += common.h \
    array.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
