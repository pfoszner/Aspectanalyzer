#ifndef KULLBACKLEIBLER_H
#define KULLBACKLEIBLER_H

#include "nmf.h"

class KullbackLeibler : public NMF
{
public:
    KullbackLeibler(std::shared_ptr<Matrix>& matrixData);
    virtual double DivernegceValue();
    virtual void PreProcessing();
    virtual void UpdateStep();
};

#endif // KULLBACKLEIBLER_H
