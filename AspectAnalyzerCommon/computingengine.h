#ifndef COMPUTINGENGINE_H
#define COMPUTINGENGINE_H
#include <QString>
#include "matrix.h"
//#include "plsa.h"
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
#include <QThreadPool>

class ComputingEngine
{
public:
    std::shared_ptr<Matrix> CurrentVmatrix;

private:
    int runningTasks;
    std::queue<std::shared_ptr<BiclusteringObject>> queue;
    std::vector<SingleThreadWorker> threadArray;
    std::queue<std::shared_ptr<BiclusteringObject>> resultsToWrite;


public:
    std::shared_ptr<DBTools> db;
    ComputingEngine();
    void LoadDataMatrix(QString filename);
    void AddBiClusteringTask(std::shared_ptr<BiclusteringObject>);
    void ServeQueue();
    int GetInQueue();
    int GetRunning();
    void CheckResultsToWrite();

private:
    QString GetHumanTime(double time);
};



#endif // COMPUTINGENGINE_H
