#ifndef PGM_H
#define PGM_H

#include "nmf.h"

class PGM : public NMF
{
    Q_OBJECT

private:
    arma::mat gradW;
    arma::mat gradH;

public:
    PGM(std::shared_ptr<Matrix>& matrixData);
    virtual double DivernegceValue();
    virtual void PreProcessing();
    virtual void UpdateStep();
};

#endif // PGM_H
