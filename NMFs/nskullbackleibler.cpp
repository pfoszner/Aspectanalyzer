#include "nskullbackleibler.h"


void nsKullbackLeibler::UpdateStep()
{
    // Multiplicative update rule
    WMatrix = WMatrix % (( dataMatrix->data / ( (WMatrix * Smatrix) * HMatrix + doubleMin )) *  (Smatrix * HMatrix).t() ) / arma::repmat(arma::sum(Smatrix * HMatrix,1).t(), p, 1);
    HMatrix = HMatrix % (  (WMatrix * Smatrix).t() * ( dataMatrix->data / ( (WMatrix * Smatrix) * HMatrix + doubleMin ))) / arma::repmat(arma::sum(WMatrix * Smatrix,0).t(), 1, n);

    // Normalization step
    HMatrix = HMatrix % arma::repmat(arma::sum(WMatrix,0).t(), 1, n);
    WMatrix = WMatrix % arma::repmat(1./arma::sum(WMatrix,0), p, 1);
}

double nsKullbackLeibler::DivernegceValue()
{    
    arma::mat WHmatrix = WMatrix * HMatrix;

    arma::mat test = ((dataMatrix->data % arma::log((dataMatrix->data / (WHmatrix + doubleMin)) + doubleMin)) - dataMatrix->data) + WHmatrix;

    return arma::accu(test);
}

void nsKullbackLeibler::InitializeSmatrix()
{
    Smatrix = arma::zeros<arma::mat>(expectedBiClusterCount, expectedBiClusterCount);

    double IdentityMatrix = 1 - theta;

    double add = theta / expectedBiClusterCount;

    for (int i = 0; i < expectedBiClusterCount; ++i)
    {
        for (int j = 0; j < expectedBiClusterCount; ++j)
        {
            if (i == j)
            {
                Smatrix(i, j) = IdentityMatrix + add;
            }
            else
            {
                Smatrix(i, j) = add;
            }
        }
    }
}

void nsKullbackLeibler::PreProcessing()
{
    InitializeSmatrix();
}

nsKullbackLeibler::nsKullbackLeibler(std::shared_ptr<Matrix>& MatrixData, double theta) : NMF(MatrixData), theta(theta)
{
    idMethod = Enums::NonSmooth_KULLBACK_LIEBER;
}
