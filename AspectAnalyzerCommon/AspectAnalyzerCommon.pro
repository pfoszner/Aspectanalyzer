#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T16:27:38
#
#-------------------------------------------------

QT       -= gui
QT       += sql

DEFINES += _OSX

TARGET = AspectAnalyzerCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += aspectanalyzercommon.cpp \
    computingengine.cpp \
    dbtools.cpp \
    dbtools.result.cpp \
    dbtools.matrix.cpp \
    dbtools.bicluster.cpp \
    dbtools.feature.cpp \
    singlethreadworker.cpp \
    dbtools.label.cpp \
    experimental.cpp

HEADERS += aspectanalyzercommon.h \
    computingengine.h \
    dbtools.h \
    singlethreadworker.h \
    experimental.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../NMFs/release/ -lNMFs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../NMFs/debug/ -lNMFs
else:unix: LIBS += -L$$PWD/../NMFs/ -lNMFs

INCLUDEPATH += $$PWD/../NMFs
DEPENDPATH += $$PWD/../NMFs

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../NMFs/release/libNMFs.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../NMFs/debug/libNMFs.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../NMFs/release/NMFs.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../NMFs/debug/NMFs.lib
else:unix: PRE_TARGETDEPS += $$PWD/../NMFs/libNMFs.a

INCLUDEPATH += /usr/local/include/
DEPENDPATH += /usr/local/include
LIBS += -framework Accelerate
LIBS += -L/usr/local/lib -larmadillo
LIBS += -L/opt/local/lib

INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include
#LIBS += -L/usr/lib -larmadillo

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

DISTFILES +=

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../EnsembleBiClustering/release/ -lEnsembleBiClustering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../EnsembleBiClustering/debug/ -lEnsembleBiClustering
else:unix: LIBS += -L$$PWD/../EnsembleBiClustering/ -lEnsembleBiClustering

INCLUDEPATH += $$PWD/../EnsembleBiClustering
DEPENDPATH += $$PWD/../EnsembleBiClustering

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../EnsembleBiClustering/release/libEnsembleBiClustering.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../EnsembleBiClustering/debug/libEnsembleBiClustering.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../EnsembleBiClustering/release/EnsembleBiClustering.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../EnsembleBiClustering/debug/EnsembleBiClustering.lib
else:unix: PRE_TARGETDEPS += $$PWD/../EnsembleBiClustering/libEnsembleBiClustering.a
