#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:26:38
#
#-------------------------------------------------

QT       -= gui

TARGET = MAPCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += mapcommon.cpp

HEADERS += mapcommon.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
