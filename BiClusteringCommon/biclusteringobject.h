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
#include <QDir>
//#include <armadillo>
//#include "/usr/local/include/armadillo"

class BiclusteringObject : public QObject
{
    Q_OBJECT

public :

enum ComputingMode
{
    Local = 0,
    RemoteToCompute,
    ReadyToSend,
    RemoteDone,
};

    int idMatrix = -1;
    int idResult = -1;
    int idMethod = -1;
    int expectedBiClusterCount;
    double time_spent;
    ComputingMode mode = Local;
    QString sourceAddress = "";
    std::shared_ptr<Matrix> dataMatrix;
    std::vector<std::shared_ptr<Bicluster>> foundedBiclusters;
    std::vector<FeatureResult> features;
    void GenerateARFFFile(QString path, int dim, std::vector<int> indexes = std::vector<int>());
    QString saveToLocalFile = "";
    QString desc = "";

public:
    BiclusteringObject(QByteArray);
    BiclusteringObject(std::shared_ptr<Matrix>&);
    BiclusteringObject(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult, double time, QString desc)
        : BiclusteringObject(Vmatrix)
    {
        idMethod = Method;
        idResult = IdResult;
        time_spent = time;
        this->desc = desc;
    }

    QByteArray Serialize(bool withData);
    void Deserialize(QByteArray deserialize);

    virtual std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
    virtual void ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>&);
    Array<double> GetCostMatrixForBiclusters(const std::vector<std::shared_ptr<Bicluster>>&, const std::vector<std::shared_ptr<Bicluster>>&, Enums::BiclusterCompareMode, Enums::SimilarityMethods);

    //virtual ~BiclusteringObject(){}

//protected:
    void PostProcessingTask();

    void SaveToLocalFile(double AverageAVC, double Similarity);

    double Recovery();

    double Relevance();

private:
    double RecoveryRelevance(const std::vector<std::shared_ptr<Bicluster>>& first, const std::vector<std::shared_ptr<Bicluster>>& second);

    std::vector<Enums::FeatureType> ft;

signals:
    void ReportProgress(int steps);
};

typedef std::shared_ptr<BiclusteringObject> ResultPointer;

#endif // BICLUSTERINGOBJECT_H
