#ifndef BICLUSTERINGUTILS_H
#define BICLUSTERINGUTILS_H

#include "biclusteringobject.h"

class BiClusteringUtils
{
public:
    BiClusteringUtils();

    static Array<double> GetCostMatrixForBiclusters(std::vector<std::shared_ptr<Bicluster>>&, std::vector<std::shared_ptr<Bicluster>>&, Enums::BiclusterCompareMode, Enums::SimilarityMethods);
};

#endif // BICLUSTERINGUTILS_H
