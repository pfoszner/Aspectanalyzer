#ifndef QUBIC_H
#define QUBIC_H

#include "biclusteringobject.h"

class QUBIC : BiclusteringObject
{
public:
    QUBIC::QUBIC(std::shared_ptr<Matrix>& data) : BiclusteringObject(data)
    {

    }

    std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);


};

#endif // QUBIC_H
