#ifndef CONSENSUSWORKER_H
#define CONSENSUSWORKER_H


#include <QThread>
#include <QRunnable>
#include <QMutex>
#include <queue>
#include "ensembletask.h"


class ConsensusWorker : public QRunnable
{

private:
    std::shared_ptr<Bicluster>& bic;
    std::shared_ptr<Matrix> dataMatrix;
    std::vector<std::shared_ptr<Bicluster>> connected;
    MergeType extractType = MergeType::Standard;
    Enums::FeatureType choosenMeasure;
    double orgResultsSize;
    int minClusterSize;
    double percentage;
    long penaltyForGainFunction = 1;

public:
    ConsensusWorker(std::shared_ptr<Bicluster>& bic, std::shared_ptr<Matrix> dataMatrix, const std::vector<std::shared_ptr<Bicluster>>& connected, MergeType extractType, Enums::FeatureType qualityMeasure, long penaltyForGainFunction, double orgResultsSize, int minClusterSize, double percentage)
        : bic(bic), dataMatrix(dataMatrix), connected(connected), extractType(extractType), choosenMeasure(qualityMeasure), orgResultsSize(orgResultsSize), minClusterSize(minClusterSize), percentage(percentage), penaltyForGainFunction(penaltyForGainFunction)
    {

    }

    void run();

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

};




#endif // CONSENSUSWORKER_H
