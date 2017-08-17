#ifndef PLSA_H
#define PLSA_H

#include "nmf.h"

class PLSA : public NMF
{
    Q_OBJECT

private:
    /// <summary>
    /// P( z_k | d_i , w_j )
    /// </summary>
    arma::cube AspectDocWord;

    /// <summary>
    /// n( d_i ) - the number of occurrences of all words in the document
    /// </summary>
    arma::mat NumberWords;

    bool IsNormalized = false;

public:
    PLSA(std::shared_ptr<Matrix>&);

protected:
    void InitializatePLSA();
    void E_step();
    void M_step();
    double DivernegceValue();
    void NormaizeDataMatrix();
    void PreProcessing();
    void UpdateStep();
};

#endif // PLSA_H
