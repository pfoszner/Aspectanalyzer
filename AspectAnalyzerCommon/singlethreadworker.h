#ifndef SINGLETHREADWORKER_H
#define SINGLETHREADWORKER_H

#include <QThread>
#include "biclusteringobject.h"
#include <QRunnable>
#include <QMutex>
#include <queue>

class SingleThreadWorker : public QRunnable
{
public:
    std::queue<std::shared_ptr<BiclusteringObject>>& tmpQueue;
    QMutex m;
    std::shared_ptr<BiclusteringObject> task;
    int s;

public:
    SingleThreadWorker(std::queue<std::shared_ptr<BiclusteringObject>>&, std::shared_ptr<BiclusteringObject>, int);

    void run();
};

#endif // SINGLETHREADWORKER_H
