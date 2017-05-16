#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:25:58
#
#-------------------------------------------------

QT       -= gui

TARGET = HungarianAlgorithm
TEMPLATE = lib
CONFIG += staticlib

DEFINES += _OSX

SOURCES += hungarianalgorithm.cpp \
    classicalhungarian.cpp

HEADERS += hungarianalgorithm.h \
    classicalhungarian.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Common/release/ -lCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Common/debug/ -lCommon
else:unix: LIBS += -L$$PWD/../Common/ -lCommon

INCLUDEPATH += $$PWD/../Common
DEPENDPATH += $$PWD/../Common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Common/release/libCommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Common/debug/libCommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../Common/release/Common.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../Common/debug/Common.lib
else:unix: PRE_TARGETDEPS += $$PWD/../Common/libCommon.a

INCLUDEPATH += /usr/local/include/
DEPENDPATH += /usr/local/include
#LIBS += -framework Accelerate
#LIBS += -L/usr/local/lib -larmadillo
LIBS += -L/opt/local/lib
