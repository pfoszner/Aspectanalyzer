#ifndef COSTMATRIXWORKER_H
#define COSTMATRIXWORKER_H


#include <QThread>
#include "biclusteringobject.h"
#include <QRunnable>
#include <QMutex>
#include <queue>

class CostMatrixWorker : public QRunnable
{
public:
    double* cm;
    std::shared_ptr<Bicluster> bic1;
    std::shared_ptr<Bicluster> bic2;
    Enums::SimilarityMethods simMethod;
    Enums::BiclusterCompareMode mode;


public:
    CostMatrixWorker(double* cmItem, std::shared_ptr<Bicluster> bic1, std::shared_ptr<Bicluster> bic2, Enums::BiclusterCompareMode mode, Enums::SimilarityMethods simMethod)
        : cm(cmItem), bic1(bic1), bic2(bic2), simMethod(simMethod), mode(mode)
    {

    }

    void run();
};

#endif // COSTMATRIXWORKER_H
