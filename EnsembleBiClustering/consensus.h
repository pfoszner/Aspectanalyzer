#ifndef CONSENSUS_H
#define CONSENSUS_H

#include "ensembletask.h"
#include "consensusworker.h"


class Consensus : public EnsembleTask
{
    Q_OBJECT

public:

    MergeType ExtractType = MergeType::Standard;
    long penaltyForGainFunction = 1;
    Enums::FeatureType qualityMeasure = Enums::FeatureType::ACV;

    Consensus(std::shared_ptr<Matrix>& Vmatrix, int IdResult, double time, QString desc)
        : EnsembleTask(Vmatrix, Enums::Methods::CONSENSUS, IdResult, time, desc)
    {

    }

    std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& results, double percentage, uint minClusterSize);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& Results, double percentage, uint minClusterSize, int maxBiclusterNum);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& results, double percentage, uint minClusterSize);
    std::vector<std::vector<std::shared_ptr<Bicluster>>> SortResults(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& UnSortedResults);
    std::vector<std::shared_ptr<Bicluster>> CreateGenericBiclusters(std::vector<std::vector<std::shared_ptr<Bicluster>>> orgResults, double percentage, uint minClusterSize, int maxBiclusterNum);






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
