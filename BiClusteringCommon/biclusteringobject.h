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
//#include <armadillo>
//#include "/usr/local/include/armadillo"

class BiclusteringObject
{

public:
    int idResult = -1;
    int idMethod = -1;
    int expectedBiClusterCount;
    std::shared_ptr<Matrix> dataMatrix;
    std::vector<std::shared_ptr<Bicluster>> foundedBiclusters;
    double time_spent;
    std::vector<FeatureResult> features;


public:
    BiclusteringObject(std::shared_ptr<Matrix>&);
    BiclusteringObject(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult, double time)
        : BiclusteringObject(Vmatrix)
    {
        idMethod = Method;
        idResult = IdResult;
        time_spent = time;
    }

    virtual std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
    virtual void ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>&);
    Array<double> GetCostMatrixForBiclusters(const std::vector<std::shared_ptr<Bicluster>>&, const std::vector<std::shared_ptr<Bicluster>>&, Enums::BiclusterCompareMode, Enums::SimilarityMethods);

    virtual ~BiclusteringObject(){}

protected:
    void PostProcessingTask();
};

#endif // BICLUSTERINGOBJECT_H
