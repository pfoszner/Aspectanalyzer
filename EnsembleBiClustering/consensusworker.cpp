#include "consensusworker.h"

void ConsensusWorker::run()
{
    if (dataMatrix != nullptr)
    {
        for( std::shared_ptr<Bicluster>& item : connected)
        {
            if (item->cluster1.size() > 0 && item->cluster2.size() > 0)
                item->ACV = dataMatrix->AverageCorrelationValue(item->cluster1, item->cluster2);
            else
                item->ACV = 0;
        }
    }

    std::vector<ClusterItem> Cluster1;
    std::vector<ClusterItem> Cluster2;

    for ( std::shared_ptr<Bicluster>& item : connected)
    {
        for (int c1 : item->cluster1)
        {
            auto it = std::find_if(Cluster1.begin(), Cluster1.end(), [c1](ClusterItem iter) { return iter.Index == c1; } );

            if (it != Cluster1.end())
                it->IncreaseWeight(*item->ACV);
            else
                Cluster1.emplace_back(c1, *item->ACV);
        }

        for (int c2 : item->cluster2)
        {
            auto it = std::find_if(Cluster2.begin(), Cluster2.end(), [c2](ClusterItem iter) { return iter.Index == c2; } );

            if (it != Cluster2.end())
                it->IncreaseWeight(*item->ACV);
            else
                Cluster2.emplace_back(c2, *item->ACV);
        }
    }

    if (extractType == MergeType::Standard)
    {
        double MinWeight = orgResultsSize;

        std::vector<int> FinalCluster1;
        std::vector<int> FinalCluster2;

        bool done = false;

        while (!done)
        {
            FinalCluster1.clear();

            for (ClusterItem item : Cluster1)
            {
                if (item.Weight() >= MinWeight)
                {
                    FinalCluster1.push_back(item.Index);
                }
            }

            if ((FinalCluster1.size() >= minClusterSize) || (MinWeight <= 0.1))
                done = true;
            else
                MinWeight *= 0.9;
        }

        MinWeight = (int)std::round(orgResultsSize * percentage);

        done = false;

        while (!done)
        {
            FinalCluster2.clear();

            for (ClusterItem item : Cluster2)
            {
                if (item.Weight() >= MinWeight)
                {
                    FinalCluster2.push_back(item.Index);
                }
            }

            if ((FinalCluster2.size() >= minClusterSize) || (MinWeight <= 0.1))
                done = true;
            else
                MinWeight *= 0.9;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);



    }
    else if (extractType == MergeType::ByACV)
    {
        std::vector<int> FinalCluster1(Cluster1.size());
        std::vector<int> FinalCluster2(Cluster2.size());

        //TOPTip: GPervTools & TCMalloc

        std::transform(Cluster1.begin(), Cluster1.end(), FinalCluster1.begin(), [](const ClusterItem& c){ return c.Index; });
        std::transform(Cluster2.begin(), Cluster2.end(), FinalCluster2.begin(), [](const ClusterItem& c){ return c.Index; });

        bool done = false;

        Bicluster Final(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);

        double currentValue = *Final.ACV;
        std::shared_ptr<double> candidateValue = 0;

        while (!done)
        {
            if (FinalCluster1.size() <= minClusterSize)
            {
                Cluster1.clear();
            }

            if (FinalCluster2.size() <= minClusterSize)
            {
                Cluster2.clear();
            }

            if (Cluster1.size() > 0)
            {
                auto iter = std::min_element(Cluster1.begin(), Cluster1.end(), [](const ClusterItem& c1, const ClusterItem& c2){ return c1.Index < c2.Index; });
                int Index = iter->Index;

                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [Index](int r){ return r == Index; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                auto criter = std::remove_if(Cluster1.begin(), Cluster1.end(), [Index](ClusterItem& r) { return r.Index == Index; });
                Cluster1.erase(criter, Cluster1.end());

                candidateValue = dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2);

                qDebug() << "ByACV: Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size();

                if (*candidateValue <= currentValue)
                {
                    FinalCluster1.push_back(Index);
                }
                else
                {
                    currentValue = *candidateValue;
                }
            }

            if (Cluster2.size() > 0)
            {
                auto iter = std::min_element(Cluster2.begin(), Cluster2.end(), [](const ClusterItem& c1, const ClusterItem& c2){ return c1.Index < c2.Index; });
                int Index = iter->Index;

                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [Index](int r){ return r == Index; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                auto criter = std::remove_if(Cluster2.begin(), Cluster2.end(), [Index](const ClusterItem& r) { return r.Index == Index; });
                Cluster2.erase(criter, Cluster2.end());

                candidateValue = dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2);

                qDebug() << "ByACV: Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size();

                if (*candidateValue <= currentValue)
                {
                    FinalCluster2.push_back(Index);
                }
                else
                {
                    currentValue = *candidateValue;
                }
            }

            if (Cluster1.size() == 0 && Cluster2.size() == 0)
                done = true;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);

    }
    else if (extractType == MergeType::ByACVHeuristic)
    {
        std::vector<int> FinalCluster1(Cluster1.size());
        std::vector<int> FinalCluster2(Cluster2.size());

        //TOPTip: GPervTools & TCMalloc

        std::transform(Cluster1.begin(), Cluster1.end(), FinalCluster1.begin(), [](const ClusterItem& c){ return c.Index; });
        std::transform(Cluster2.begin(), Cluster2.end(), FinalCluster2.begin(), [](const ClusterItem& c){ return c.Index; });

        bool done = false;

        while (!done)
        {
            double min1 = std::numeric_limits<double>::max();
            double min2 = std::numeric_limits<double>::max();

            int index1 = -1;
            int index2 = -1;

            if (FinalCluster1.size() <= minClusterSize)
            {
                Cluster1.clear();
            }

            if (FinalCluster2.size() <= minClusterSize)
            {
                Cluster2.clear();
            }

            std::shared_ptr<double> currentValue = dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2);

            if (Cluster1.size() > 0)
            {
                auto iter = std::min_element(Cluster1.begin(), Cluster1.end(), [](ClusterItem& c1, ClusterItem& c2){ return c1.Weight() < c2.Weight(); });

                index1 = iter->Index;

                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [index1](const int r) { return r == index1; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                std::shared_ptr<double> Candidate =  dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2);

                min1 = *Candidate - *currentValue;

                FinalCluster1.push_back(index1);
            }

            if (Cluster2.size() > 0)
            {
                auto iter = std::min_element(Cluster2.begin(), Cluster2.end(), [](const ClusterItem& c1,  const ClusterItem& c2){ return c1.Weight() < c2.Weight(); });

                index2 = iter->Index;

                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [index2](int r) { return r == index2; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                std::shared_ptr<double> Candidate = dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2);

                min2 = *Candidate - *currentValue;

                FinalCluster2.push_back(index2);
            }

            if ((index1 > -1) && ((min1 > 0 && min1 >= min2) || (min1 > 0 && min2 == std::numeric_limits<double>::max())))
            {
                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [index1](int r) { return r == index1; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                auto criter = std::remove_if(Cluster1.begin(), Cluster1.end(), [index1](const ClusterItem& r) { return r.Index == index1; });
                Cluster1.erase(criter, Cluster1.end());
            }
            else if (min1 <= 0 && min1 < std::numeric_limits<double>::max())
            {
                auto riter = std::remove_if(Cluster1.begin(), Cluster1.end(), [index1](const ClusterItem& r) { return r.Index == index1; });
                Cluster1.erase(riter, Cluster1.end());
            }

            if ((index2 > -1) && ((min2 > 0 && min2 >= min1) || (min2 > 0 && min1 == std::numeric_limits<double>::max())))
            {
                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [index2](int r) { return r == index2; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                auto criter = std::remove_if(Cluster2.begin(), Cluster2.end(), [index2](const ClusterItem& r) { return r.Index == index2; });
                Cluster2.erase(criter, Cluster2.end());
            }
            else if (min2 <= 0 && min2 < std::numeric_limits<double>::max())
            {
                auto riter = std::remove_if(Cluster2.begin(), Cluster2.end(), [index2](const ClusterItem& r) { return r.Index == index2; });
                Cluster2.erase(riter, Cluster2.end());
            }

            qDebug() << "ByACVHeuristic: Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size();

            if (Cluster1.size() == 0 && Cluster2.size() == 0)
                done = true;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);

    }
}
