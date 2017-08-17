#ifndef BICLUSTERINGOBJECT_H
#define BICLUSTERINGOBJECT_H

#include "matrix.h"
#include "featureresult.h"
#include "bicluster.h"
#include <iostream>
#include <memory>
#include "common.h"
#include "biclusteringutils.h"
#include "enums.h"
#include "classicalhungarian.h"
#include <QDebug>
#include <QThreadPool>
#include "costmatrixworker.h"
#include <QDataStream>
//#include <armadillo>
//#include "/usr/local/include/armadillo"

class BiclusteringObject : public QObject
{

    Q_OBJECT

public:
    int idMatrix = -1;
    int idResult = -1;
    int idMethod = -1;
    int expectedBiClusterCount;
    double time_spent;
    QString sourceAddress = "";
    std::shared_ptr<Matrix> dataMatrix;
    std::vector<std::shared_ptr<Bicluster>> foundedBiclusters;
    std::vector<FeatureResult> features;
    void GenerateARFFFile(QString path, int dim, std::vector<int> indexes = std::vector<int>());

public:
    BiclusteringObject(QByteArray);
    BiclusteringObject(std::shared_ptr<Matrix>&);
    BiclusteringObject(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult, double time)
        : BiclusteringObject(Vmatrix)
    {
        idMethod = Method;
        idResult = IdResult;
        time_spent = time;
    }

    QByteArray Serialize();


    virtual std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
    virtual void ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>&);
    Array<double> GetCostMatrixForBiclusters(const std::vector<std::shared_ptr<Bicluster>>&, const std::vector<std::shared_ptr<Bicluster>>&, Enums::BiclusterCompareMode, Enums::SimilarityMethods);

    virtual ~BiclusteringObject(){}



//protected:
    void PostProcessingTask();

signals:
    void ReportProgress(int steps);
};

#endif // BICLUSTERINGOBJECT_H
