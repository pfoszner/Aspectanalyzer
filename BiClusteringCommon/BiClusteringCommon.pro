#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:18:46
#
#-------------------------------------------------

TARGET = BiClusteringCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += biclusteringcommon.cpp \
    matrix.cpp \
    biclusteringobject.cpp \
    featureresult.cpp \
    enums.cpp \
    bicluster.cpp \
    biclusteringutils.cpp \
    label.cpp \
    costmatrixworker.cpp

HEADERS += biclusteringcommon.h \
    matrix.h \
    biclusteringobject.h \
    featureresult.h \
    enums.h \
    bicluster.h \
    biclusteringutils.h \
    label.h \
    costmatrixworker.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../References/armadillo-7.300.1/release/ -larmadillo
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../References/armadillo-7.300.1/debug/ -larmadillo
#else:unix: LIBS += -L$$PWD/../References/armadillo-7.300.1/ -larmadillo
#else:unix: LIBS += /usr/local/lib -larmadillo

#INCLUDEPATH += $$PWD/../References/armadillo-7.300.1
#DEPENDPATH += $$PWD/../References/armadillo-7.300.1

#INCLUDEPATH += /usr/local/include/
#DEPENDPATH += /usr/local/include
#LIBS += -framework Accelerate
#LIBS += -L/usr/local/lib -larmadillo

INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include
LIBS += -L/usr/lib -larmadillo

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
