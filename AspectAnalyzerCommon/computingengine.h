#ifndef COMPUTINGENGINE_H
#define COMPUTINGENGINE_H
#include <QString>
#include "matrix.h"
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <thread>
#include <memory>
#include <queue>
#include <QThread>
#include <future>         // std::async, std::future
#include <exception>      // std::exception
#include "common.h"
#include "dbtools.h"
#include "singlethreadworker.h"
#include "server.h"
#include "client.h"
#include <QThreadPool>
#include <QObject>

class ComputingEngine : public QObject
{
    Q_OBJECT

public:
    std::shared_ptr<Matrix> CurrentVmatrix;
    Client aaclient;
    int maxThreadAllowd = 8;

private:
    int runningTasks;
    std::queue<std::shared_ptr<BiclusteringObject>> queue;
    std::vector<SingleThreadWorker> threadArray;
    std::queue<std::shared_ptr<BiclusteringObject>> resultsToWrite;

public:
    std::shared_ptr<DBTools> db;
    explicit ComputingEngine(QObject *parent = 0);
    void LoadDataMatrix(QString filename);
    void AddBiClusteringTask(std::shared_ptr<BiclusteringObject>);
    void ServeQueue();
    int GetInQueue();
    int GetRunning();
    void CheckResultsToWrite();

private:
    QString GetHumanTime(double time);

public slots:
    void receiveData(QByteArray);
};



#endif // COMPUTINGENGINE_H
