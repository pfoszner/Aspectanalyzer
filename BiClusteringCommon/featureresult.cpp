#include "featureresult.h"

FeatureResult::FeatureResult(Enums::FeatureType type, double value, int indexNbr) : indexNbr(indexNbr), value(value), type(type)
{

}

FeatureResult::FeatureResult(Enums::FeatureType type, double value, int indexNbr, int idFeature, int idResult, int idBicluster, int idMatrix) : FeatureResult(type, value, indexNbr)
{
    this->idFeature = idFeature;
    this->idResult = idResult;
    this->idBicluster = idBicluster;
    this->idMatrix = idMatrix;
}
