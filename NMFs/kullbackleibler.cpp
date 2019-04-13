#include "kullbackleibler.h"


KullbackLeibler::KullbackLeibler(std::shared_ptr<Matrix> MatrixData) : NMF(MatrixData)
{
    idMethod = Enums::KULLBACK_LIEBER;
}

void KullbackLeibler::UpdateStep()
{
    // Multiplicative update rule
    WMatrix = WMatrix % (( dataMatrix->data / ( WMatrix * HMatrix + doubleMin )) *  HMatrix.t() ) / arma::repmat(arma::sum(HMatrix,1).t(), p, 1);
    HMatrix = HMatrix % (  WMatrix.t() * ( dataMatrix->data / ( WMatrix * HMatrix + doubleMin ))) / arma::repmat(arma::sum(WMatrix,0).t(), 1, n);

    // Normalization step
    HMatrix = HMatrix % arma::repmat(arma::sum(WMatrix,0).t(), 1, n);
    WMatrix = WMatrix % arma::repmat(1./arma::sum(WMatrix,0), p, 1);
}

double KullbackLeibler::DivernegceValue()
{
    arma::mat WHmatrix = WMatrix * HMatrix;

    arma::mat test = ((dataMatrix->data % arma::log((dataMatrix->data / (WHmatrix + doubleMin)) + doubleMin)) - dataMatrix->data) + WHmatrix;

    return arma::accu(test);
}

void KullbackLeibler::PreProcessing()
{

}
