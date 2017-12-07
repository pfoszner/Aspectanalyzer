#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T15:26:13
#
#-------------------------------------------------

QT       -= gui

TARGET = BiClusteringAlgorithms
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    qubic.cpp

HEADERS += \
    qubic.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../BiClusteringCommon/release/ -lBiClusteringCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../BiClusteringCommon/debug/ -lBiClusteringCommon
else:unix: LIBS += -L$$PWD/../BiClusteringCommon/ -lBiClusteringCommon

INCLUDEPATH += $$PWD/../BiClusteringCommon
DEPENDPATH += $$PWD/../BiClusteringCommon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../BiClusteringCommon/release/libBiClusteringCommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../BiClusteringCommon/debug/libBiClusteringCommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../BiClusteringCommon/release/BiClusteringCommon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../BiClusteringCommon/debug/BiClusteringCommon.lib
else:unix: PRE_TARGETDEPS += $$PWD/../BiClusteringCommon/libBiClusteringCommon.a

INCLUDEPATH += /usr/local/include/
DEPENDPATH += /usr/local/include
#LIBS += -framework Accelerate
#LIBS += -L/usr/local/lib -larmadillo
LIBS += -L/opt/local/lib

INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include
#LIBS += -L/usr/lib -larmadillo

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../HungarianAlgorithm/release/ -lHungarianAlgorithm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../HungarianAlgorithm/debug/ -lHungarianAlgorithm
else:unix: LIBS += -L$$PWD/../HungarianAlgorithm/ -lHungarianAlgorithm

INCLUDEPATH += $$PWD/../HungarianAlgorithm
DEPENDPATH += $$PWD/../HungarianAlgorithm

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../HungarianAlgorithm/release/libHungarianAlgorithm.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../HungarianAlgorithm/debug/libHungarianAlgorithm.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../HungarianAlgorithm/release/HungarianAlgorithm.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../HungarianAlgorithm/debug/HungarianAlgorithm.lib
else:unix: PRE_TARGETDEPS += $$PWD/../HungarianAlgorithm/libHungarianAlgorithm.a
