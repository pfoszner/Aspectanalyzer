#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:27:00
#
#-------------------------------------------------

QT       -= gui

TARGET = Pierscalla
TEMPLATE = lib
CONFIG += staticlib

SOURCES += pierscalla.cpp

HEADERS += pierscalla.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
