#include "lse.h"

LSE::LSE(std::shared_ptr<Matrix>& matrixData) : NMF(matrixData)
{
    idMethod = Enums::LEAST_SQUARE_ERROR;
}

void LSE::UpdateStep()
{
    HMatrix = (HMatrix % ((WMatrix.t() * dataMatrix->data) / (((WMatrix.t() * WMatrix) * HMatrix) + doubleMin)));
    WMatrix = (WMatrix % ((dataMatrix->data * HMatrix.t()) / (((WMatrix * HMatrix) * HMatrix.t()) + doubleMin)));
}

double LSE::DivernegceValue()
{
    arma::mat powComponent = dataMatrix->data - (WMatrix * HMatrix);

    return arma::accu(powComponent % powComponent);
}

void LSE::PreProcessing()
{
}
