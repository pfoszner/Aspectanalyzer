
#include "plsa.h"


PLSA::PLSA(std::shared_ptr<Matrix>& matrixData) : NMF(matrixData)
{
    idMethod = Enums::PLSA;
}

void PLSA::InitializatePLSA()
{
    // P( z_k | d_i , w_j )
    AspectDocWord = arma::zeros<arma::cube>(p, n, expectedBiClusterCount);
    // n( d_i ) - //Sum up the number of occurrences of words in documents
    NumberWords = arma::sum(dataMatrix->data, 0).t();
}

/// <summary>
/// E step from EM algorithm
///
///                              P( w_j | z_k)*P( z_k | d_i )
/// P( z_k | d_i, w_j ) = --------------------------------------------
///                       SUM(l = 1...K){P( w_j | z_l)*P( z_l | d_i )}
///
/// K - number of aspects
///
/// </summary>
void PLSA::E_step()
{
    arma::mat Denominator = (WMatrix * HMatrix) + doubleMin;

    for (int l = 0; l < expectedBiClusterCount; ++l)
    {
        AspectDocWord.slice(l) = (WMatrix.col(l) * HMatrix.row(l)) / Denominator;
    }
}

/// <summary>
/// M step from EM algorithm
///
///                         SUM(i=1...N){n( d_i, w_j )*P( z_k | d_i, w_j )}
/// P( w_j | z_k ) = -------------------------------------------------------------
///                  SUM(m=1...M){SUM(i=1...N){n( d_i, w_m )*P( z_k | d_i, w_m )}}
///
///
///                   SUM(j=1...M){n( d_i, w_j )*P( z_k | d_i, w_j )}
/// P ( z_k | d_i ) = -----------------------------------------------
///                                      n( d_i )
///
/// M - number of words
/// N - number of documents
///
/// </summary>
void PLSA::M_step()
{
    arma::mat Tmp;// = arma::zeros<arma::mat>(p, n);

    arma::vec Denominator;// = arma::zeros<arma::vec>(1);

    for (int l = 0; l < expectedBiClusterCount; ++l)
    {
        Tmp = AspectDocWord.slice(l);

        Denominator = arma::accu(dataMatrix->data % Tmp) + doubleMin;

        WMatrix.col(l) = (arma::sum((dataMatrix->data % Tmp), 1) / Denominator(0,0));

        HMatrix.row(l) = (arma::sum((dataMatrix->data % Tmp), 0) / (NumberWords.t() + doubleMin));
    }

    // Normalization step
    HMatrix = HMatrix % arma::repmat(arma::sum(WMatrix,0).t(), 1, n);
    WMatrix = WMatrix % arma::repmat(1./arma::sum(WMatrix,0), p, 1);
}

/// <summary>
/// Method computes log likelihood after M-step EM algorithm
/// </summary>
/// <returns>Log-likelihood</returns>
double PLSA::DivernegceValue()
{
    // calculate likelihood and update p(term, doc)
    arma::mat WHMatrix  = WMatrix * HMatrix;
    return arma::sum(arma::sum(dataMatrix->data % arma::log(WHMatrix + doubleMin)));
}

void PLSA::NormaizeDataMatrix()
{
    double Min = dataMatrix->data.min();
    double Max = dataMatrix->data.max();

    dataMatrix->data = (dataMatrix->data - Min) / (Max - Min);

    IsNormalized = true;
}

void PLSA::PreProcessing()
{
    InitializatePLSA();

    if (!IsNormalized)
        NormaizeDataMatrix();
}

void PLSA::UpdateStep()
{
    E_step();

    M_step();
}
