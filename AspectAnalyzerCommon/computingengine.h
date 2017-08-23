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
    int runningTasks = 0;
    int taskToComputute = 0;
    std::queue<ResultPointer> queue;
    std::vector<SingleThreadWorker> threadArray;
    std::queue<ResultPointer> resultsToWrite;
    int progressSteps = 0;
    int currentProgressSteps = 0;
    time_t queueStart;
    QMutex lock;

public:
    std::shared_ptr<DBTools> db;
    explicit ComputingEngine(QObject *parent = 0);
    void LoadDataMatrix(QString filename);
    void AddBiClusteringTask(ResultPointer);
    void ServeQueue();
    int GetInQueue();
    int GetRunning();
    void CheckResultsToWrite();

private:
    QString GetHumanTime(double time);

public slots:
    void receiveData(QByteArray);
    void UpdateProgress(int);
    void CheckWriteResult(ResultPointer);

signals:
    void setProgressChange(const int newValue, const QString eta);
    void setTasksLabels(const QString &running, const QString &inqueue);
};



#endif // COMPUTINGENGINE_H
