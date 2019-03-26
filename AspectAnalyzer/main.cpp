#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "buttonhandlers.h"
#include "addbiclusteringtask.h"
#include "server.h"

int main(int argc, char *argv[])
{
    ButtonHandlers btnHandlers;

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("lblreceiver", &btnHandlers);
    ctx->setContextProperty("lblTasks", btnHandlers.engine.get());
    ctx->setContextProperty("lblProgressBar", btnHandlers.engine.get());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    //QObjectList test = window->children();

    //QObject dataTab = test[1]->children();

    //QObject *dataView = dataTab.children().value(0);
    //QTableView *dataViewObject = qobject_cast<QTableView *>(dataView);

    QObject::connect(window, SIGNAL(btnAddTaskSignal()),
                         &btnHandlers, SLOT(btnAddTaskSlot()));

//    QObject::connect(window, SIGNAL(btnLoadFromFileSignal()),
//                         &btnHandlers, SLOT(btnLoadFromFileSlot()));

//    QObject::connect(window, SIGNAL(btnLoadFromDatabaseSignal()),
//                         &btnHandlers, SLOT(btnLoadFromDatabaseSlot()));

//    QObject::connect(window, SIGNAL(btnTuneSignal()),
//                         &btnHandlers, SLOT(btnTuneSlot()));

    QObject::connect(window, SIGNAL(btnPlaySignal()),
                         &btnHandlers, SLOT(btnPlaySlot()));

    QObject::connect(window, SIGNAL(btnPauseSignal()),
                         &btnHandlers, SLOT(btnPauseSlot()));

    QObject::connect(window, SIGNAL(btnStopSignal()),
                         &btnHandlers, SLOT(btnStopSlot()));

    QObject::connect(window, SIGNAL(btnCustomSignal(QString)),
                         &btnHandlers, SLOT(btnCustomSlot(QString)));

    QObject::connect(window, SIGNAL(btnLoadResultsSignal()),
                         &btnHandlers, SLOT(btnLoadResultsSlot()));

    Server aaServer;

    QObject::connect(&aaServer, &Server::dataReceived,
                         btnHandlers.engine.get(), &ComputingEngine::receiveData);

    return app.exec();
}
