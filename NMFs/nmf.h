#ifndef NMF_H
#define NMF_H

#include "biclusteringobject.h"
#include "nmf.h"
#include "biclusteringcommon.h"
#include "enums.h"
#include <cstdlib>
#include <armadillo>

class NMF : public BiclusteringObject, public std::enable_shared_from_this<NMF>
{
    Q_OBJECT

public:
    NMF(std::shared_ptr<Matrix>&);
    NMF(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult, double time, QString desc);
    NMF(QByteArray deserialize);// : BiclusteringObject(deserialize) { }
    arma::mat HMatrix;
    arma::mat WMatrix;


protected:
    arma::umat WBiclusters;
    arma::umat HBiclusters;
    int SupervisedDim = -1;
    int maxNumberOfSteps = 500;
    Enums::ExtractingMethod exMethod = Enums::Zero;
    double theta = 0.5;
    bool supervisedDim = false;
    double cutOffThreashold = 0.75;
    int n;
    int p;
    int count = 0;
    const int conectivityStepNumber = 40;
    bool useConnectivity = false;
    arma::umat old_cc;
    const double doubleMin = 10e-300;
    bool trimByACV = false;
    double divergence = 0;
    int minN = 4;
    int minP = 20;

public:
    void RebuildBiclusters();
    void SaveNMFToLocalFile();


protected:
    virtual double DivernegceValue();
    virtual void UpdateStep();
    virtual void PreProcessing();
    void CheckNonNegativity();
    void ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>&);
    void InitializateFirstValues();
    bool CheckBiClusters();
    bool ConnectivityMatrix();
    std::vector<std::shared_ptr<Bicluster>> GetBiclusters();
    int maxRowH(int);

private:
    std::vector<int> GetWBicluster(int k, Enums::ExtractingMethod extractingType, double threshold, uint maxBiclusterSize);
    std::vector<int> GetHBicluster(int k, Enums::ExtractingMethod extractingType, double threshold, uint maxBiclusterSize);
    void TrimCluster1(std::shared_ptr<Bicluster>& bic, int bicNumber);
    void TrimCluster2(std::shared_ptr<Bicluster>& bic, int bicNumber);


public:
    std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
};

#endif // NMF_H
