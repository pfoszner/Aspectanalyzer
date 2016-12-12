#ifndef CONSENSUS_H
#define CONSENSUS_H

#include "ensembletask.h"

class Consensus : public EnsembleTask
{
public:

    enum MergeType
        {
            Standard = 0,
            ByACV,
            ByACVHeuristic
        };

    MergeType ExtractType = MergeType::Standard;

    Consensus(std::shared_ptr<Matrix>& Vmatrix, int IdResult)
        : EnsembleTask(Vmatrix, Enums::Methods::CONSENSUS, IdResult)
    {

    }

    std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& results, double percentage, uint minClusterSize);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& Results, double percentage, uint minClusterSize, int maxBiclusterNum);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& results, double percentage, uint minClusterSize);
    std::vector<std::vector<std::shared_ptr<Bicluster>>> SortResults(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& UnSortedResults);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(std::vector<std::vector<std::shared_ptr<Bicluster>>> orgResults, double percentage, uint minClusterSize, int maxBiclusterNum);





class ClusterItem
{

public:
    int Index;
    uint Count;
    std::vector<double> ACV;

    ClusterItem(int Index, double Quality) : Index(Index), Count(1)
        {
            ACV.push_back(Quality);
        }

   void IncreaseWeight(double Quality)
        {
            Count++;
            ACV.push_back(Quality);
        }

   double Weight() const
        {
            return (std::accumulate(ACV.begin(), ACV.end(), 0.0) / (double)ACV.size()) * Count;
        }

};

class SortItem
{

public:
    uint x;
    uint y;
    double value;

    SortItem(int x, int y, double value) : x(x), y(y), value(value) { }
};

};

#endif // CONSENSUS_H
