#ifndef NSKULLBACKLEIBLER_H
#define NSKULLBACKLEIBLER_H

#include "nmf.h"

class nsKullbackLeibler : public NMF
{

private:
    arma::mat Smatrix;
    double theta;

private:
    void InitializeSmatrix();

public:
    nsKullbackLeibler(std::shared_ptr<Matrix>&, double);
    virtual double DivernegceValue();
    virtual void PreProcessing();
    virtual void UpdateStep();
};

#endif // NSKULLBACKLEIBLER_H
