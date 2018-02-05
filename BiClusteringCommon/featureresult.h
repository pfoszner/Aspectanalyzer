#ifndef FEATURERESULT_H
#define FEATURERESULT_H

#include "biclusteringcommon.h"
#include "enums.h"

class FeatureResult
{

public:
    int idResult = -1;
    int idBicluster = -1;
    int idMatrix = -1;
    int idFeature = -1;
    int indexNbr = 0;
    double value;
    Enums::FeatureType type;

public:
    FeatureResult(Enums::FeatureType type, double value, int indexNbr);
    FeatureResult(Enums::FeatureType type, double value, int indexNbr, int idFeature, int idResult, int idBicluster, int idMatrix);
};

#endif // FEATURERESULT_H
