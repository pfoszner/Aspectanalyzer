TEMPLATE = app

QT += qml quick widgets network
QT += sql

DEFINES += _OSX

CONFIG += c++11

SOURCES += main.cpp \
    buttonhandlers.cpp \
    addbiclusteringtask.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    Graphics/pleaseWait.gif \
    Graphics/pslogo.gif \
    Graphics/services-icon.jpg \
    Graphics/about.png \
    Graphics/add.png \
    Graphics/automation.png \
    Graphics/back.png \
    Graphics/clear.png \
    Graphics/data.png \
    Graphics/database.png \
    Graphics/delete.png \
    Graphics/downarrow.png \
    Graphics/error.png \
    Graphics/file.png \
    Graphics/Home.png \
    Graphics/Info.png \
    Graphics/intruder.png \
    Graphics/log.png \
    Graphics/merge.png \
    Graphics/no_image.png \
    Graphics/options.png \
    Graphics/paint.png \
    Graphics/pause.png \
    Graphics/pc.png \
    Graphics/play.png \
    Graphics/queue.png \
    Graphics/refresh.png \
    Graphics/report.png \
    Graphics/reset.png \
    Graphics/results.png \
    Graphics/run.png \
    Graphics/running.png \
    Graphics/save.png \
    Graphics/server.png \
    Graphics/slave.png \
    Graphics/stop.png \
    Graphics/tasks.png \
    Graphics/test.png \
    Graphics/tune.png \
    Graphics/uparrow.png \
    Graphics/update.png \
    Graphics/update2.png \
    Graphics/upload.png \
    Graphics/Warning.png \
    Settings.qml \
    Tab1.qml \
    Tab2.qml

HEADERS += \
    buttonhandlers.h \
    addbiclusteringtask.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../AspectAnalyzerCommon/release/ -lAspectAnalyzerCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../AspectAnalyzerCommon/debug/ -lAspectAnalyzerCommon
else:unix: LIBS += -L$$PWD/../AspectAnalyzerCommon/ -lAspectAnalyzerCommon

INCLUDEPATH += $$PWD/../AspectAnalyzerCommon
DEPENDPATH += $$PWD/../AspectAnalyzerCommon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../AspectAnalyzerCommon/release/libAspectAnalyzerCommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../AspectAnalyzerCommon/debug/libAspectAnalyzerCommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../AspectAnalyzerCommon/release/AspectAnalyzerCommon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../AspectAnalyzerCommon/debug/AspectAnalyzerCommon.lib
else:unix: PRE_TARGETDEPS += $$PWD/../AspectAnalyzerCommon/libAspectAnalyzerCommon.a

INCLUDEPATH += /usr/local/include/
DEPENDPATH += /usr/local/include
#LIBS += -framework Accelerate
#LIBS += -L/usr/local/lib -larmadillo
LIBS += -L/opt/local/lib

INCLUDEPATH += /usr/include/
DEPENDPATH += /usr/include
#LIBS += -L/usr/lib -larmadillo

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
