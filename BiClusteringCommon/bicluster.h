#ifndef BICLUSTER_H
#define BICLUSTER_H

#include <memory>
#include <vector>
#include <algorithm>
#include "enums.h"
#include <QString>
#include "common.h"
#include "featureresult.h"

class Bicluster
{
public:
    int idBicluster = -1;
    std::vector<int> cluster1;
    std::vector<int> cluster2;
    std::vector<FeatureResult> mesures;

public:
    Bicluster(const Bicluster& Copy);

    Bicluster(int IdBicluster, const std::vector<int>& cluster1, const std::vector<int>& cluster2);

    Bicluster(int X, int Y, int len1, int len2);

    double Compare(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double CompareFirst(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double CompareSecond(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double JaccardIndex(const std::vector<QString>&, const std::vector<QString>&);

    double JaccardIndex(const std::vector<int>&, const std::vector<int>&);

    uint LevenshteinDistance(const std::vector<int>&, const std::vector<int>&);

    std::shared_ptr<double> GetFeature(Enums::FeatureType type);

    void SetFeature(FeatureResult newFeature);

    void SetFeature(Enums::FeatureType type, double value);
};

#endif // BICLUSTER_H
