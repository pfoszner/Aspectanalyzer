#include "featureresult.h"

FeatureResult::FeatureResult(Enums::FeatureType type, double value, int indexNbr) : indexNbr(indexNbr), value(value), type(type)
{

}

FeatureResult::FeatureResult(Enums::FeatureType type, double value, int indexNbr, int idFeature, int idResult) : FeatureResult(type, value, indexNbr)
{
    this->idFeature = idFeature;
    this->idResult = idResult;
}
