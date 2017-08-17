#ifndef LSE_H
#define LSE_H

#include "nmf.h"

class LSE : public NMF, QObject
{
    Q_OBJECT

public:
    LSE(std::shared_ptr<Matrix>& matrixData);
    virtual double DivernegceValue();
    virtual void PreProcessing();
    virtual void UpdateStep();
};

#endif // LSE_H
