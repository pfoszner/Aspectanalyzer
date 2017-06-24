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
    double orgResultsSize;
    int minClusterSize;
    double percentage;

public:
    ConsensusWorker(std::shared_ptr<Bicluster>& bic, std::shared_ptr<Matrix> dataMatrix, const std::vector<std::shared_ptr<Bicluster>>& connected, MergeType extractType, double orgResultsSize, int minClusterSize, double percentage)
        : bic(bic), dataMatrix(dataMatrix), connected(connected), extractType(extractType), orgResultsSize(orgResultsSize), minClusterSize(minClusterSize), percentage(percentage)
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
