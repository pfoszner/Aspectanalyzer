#ifndef SINGLETHREADWORKER_H
#define SINGLETHREADWORKER_H

#include <QThread>
#include "biclusteringobject.h"
#include <QRunnable>
#include <QMutex>
#include <queue>

class SingleThreadWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ResultPointer task;

public:
    SingleThreadWorker(ResultPointer);

    void run();

signals:
    void ReportDone(ResultPointer);
};

#endif // SINGLETHREADWORKER_H
