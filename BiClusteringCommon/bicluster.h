#ifndef BICLUSTER_H
#define BICLUSTER_H

#include <memory>
#include <vector>
#include <algorithm>
#include "enums.h"
#include <QString>
#include "common.h"

class Bicluster
{
public:
    int idBicluster = -1;
    std::vector<int> cluster1;
    std::vector<int> cluster2;
    std::shared_ptr<double> ACV;
    std::shared_ptr<double> similarity;

public:
    Bicluster(const Bicluster& Copy);

    Bicluster(int IdBicluster, const std::vector<int>& cluster1, const std::vector<int>& cluster2, std::shared_ptr<double> ACV, std::shared_ptr<double> Similarity);

    Bicluster(int X, int Y, int len1, int len2);

    double Compare(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double CompareFirst(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double CompareSecond(std::shared_ptr<Bicluster>, Enums::SimilarityMethods);

    double JaccardIndex(const std::vector<QString>&, const std::vector<QString>&);

    double JaccardIndex(const std::vector<int>&, const std::vector<int>&);

    int LevenshteinDistance(const std::vector<int>&, const std::vector<int>&);
};

#endif // BICLUSTER_H
