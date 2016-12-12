#include "consensus.h"

std::shared_ptr<BiclusteringObject> Consensus::Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    //TODO: params
    params.clear();

    try
    {
        std::vector<std::shared_ptr<Bicluster>> consensusClusters = CreateGenericBiclusters(tasksToEnsemble, 100, 20, -1);

        for(auto item : consensusClusters)
        {
            foundedBiclusters.emplace_back(item);
        }

        PostProcessingTask();

        return shared_from_this();
    }
    catch (...)
    {
        //Logger.Log(String.Format("Error in Compute. Message: {0}, Stack Trace: {1}", ex.Message, ex.StackTrace), LogTypes.Error);

        std::shared_ptr<BiclusteringObject> null;

        return null;
    }
}


//public List<Bicluster>[] GenerateTwoDimensionalMunkres(List<List<Bicluster>> Results)
//{
//    List<Bicluster>[] RetVal = null;

//    try
//    {
//        #region Validate input
//        if (Results == null || Results.Count == 0)
//        {
//            throw new Exception("No results");
//        }
//        else if (!Results.TrueForAll(q => q.Count == Results[0].Count))
//        {
//            throw new Exception("Results inconsistent");
//        }
//        else if (Results.Count == 1)
//        {
//            return Results.ToArray();
//        }
//        #endregion

//        ClassicalHungarian MunkresEngine = new ClassicalHungarian();

//        List<int[,]> MunkresResults = new List<int[,]>();

//        for (int i = 0; i < Results.Count - 1; ++i)
//        {
//            double[,] CM = BiClusteringUtils.GetCostMatrixForBiclusters(Results[i], Results[i + 1], BiclusterCompareMode.Both, SimilarityMethods.JaccardIndex);

//            MunkresEngine.SetCostMatrix(CM, MAPCommon.Function.Max);

//            MunkresEngine.RunMunkres();

//            MunkresResults.Add((int[,])MunkresEngine.M.Clone());
//        }

//        int k = Results[0].Count;

//        int[] Indexes = new int[Results.Count];

//        RetVal = new List<Bicluster>[k];

//        for (int b = 0; b < k; ++b)
//        {
//            for (int i = 0; i < Results.Count - 1; ++i)
//            {
//                if (i == 0)
//                {
//                    Indexes[i] = b;
//                }

//                Indexes[i + 1] = -1;

//                for (int br = 0; br < k; ++br)
//                {
//                    if (MunkresResults[i][Indexes[i], br] == 1)
//                    {
//                        Indexes[i + 1] = br;
//                        break;
//                    }
//                }
//            }

//            RetVal[b] = new List<Bicluster>();
//            for (int i = 0; i < Results.Count; ++i)
//            {
//                RetVal[b].Add(Results[i][Indexes[i]]);
//            }
//        }
//    }
//    catch (Exception ex)
//    {
//        Logger.Log(String.Format("GenerateTwoDimensionalMunkres Error: {0}", ex.Message), LogTypes.Error);
//    }

//    return RetVal;
//}


//public void GenerateMultiDimensionalMunkres()
//{
//    try
//    {
//        throw new NotImplementedException();
//    }
//    catch (Exception ex)
//    {
//        Logger.Log(String.Format("GenerateMultiDimensionalMunkres Error: {0}", ex.Message), LogTypes.Error);
//    }
//}


std::vector<std::shared_ptr<Bicluster>> Consensus::CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& results, double percentage, uint minClusterSize)
{
    return CreateGenericBiclusters(results, percentage, minClusterSize, -1);
}

std::vector<std::shared_ptr<Bicluster>> Consensus::CreateGenericBiclusters(const std::vector<std::shared_ptr<BiclusteringObject>>& Results, double percentage, uint minClusterSize, int maxBiclusterNum)
{
    std::vector<std::vector<std::shared_ptr<Bicluster>>> processedResults;

    for (std::shared_ptr<BiclusteringObject> result : Results)
    {
        processedResults.emplace_back(result->foundedBiclusters);
    }

    return CreateGenericBiclusters(processedResults, percentage, minClusterSize, maxBiclusterNum);
}

std::vector<std::shared_ptr<Bicluster>> Consensus::CreateGenericBiclusters(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& results, double percentage, uint minClusterSize)
{
    return CreateGenericBiclusters(results, percentage, minClusterSize, -1);
}

std::vector<std::vector<std::shared_ptr<Bicluster>>> Consensus::SortResults(const std::vector<std::vector<std::shared_ptr<Bicluster>>>& UnSortedResults)
{
    std::vector<std::vector<std::shared_ptr<Bicluster>>> retVal;

    std::vector<Consensus::SortItem> Simi;

    ClassicalHungarian MunkresEngine;

    for (uint i = 0; i < UnSortedResults.size(); ++i)
    {
        for (uint j = 0; j < UnSortedResults.size(); ++j)
        {
            if (   (j != i)
                && (std::find_if(Simi.begin(), Simi.end(), [i,j](Consensus::SortItem q) { return q.x == i && q.y == j; } ) == Simi.end())
                && (std::find_if(Simi.begin(), Simi.end(), [i,j](Consensus::SortItem q) { return q.x == j && q.y == i; } ) == Simi.end())
               )
            {
                Array<double> CM = GetCostMatrixForBiclusters(UnSortedResults[i], UnSortedResults[j], Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

                MunkresEngine.SetCostMatrix(CM, ClassicalHungarian::Max);

                double result = MunkresEngine.RunMunkres();

                Simi.emplace_back(i, j, result);
            }
        }
    }

    auto iter = std::max_element(Simi.begin(), Simi.end(), [](Consensus::SortItem s1, Consensus::SortItem s2){ return s1.value > s2.value; });

    uint First = iter->x;
    uint Second = iter->y;

    auto riter = std::remove_if(Simi.begin(), Simi.end(), [First](Consensus::SortItem r){ return r.x == First || r.y == First; });
    Simi.erase(riter, Simi.end());
    
    retVal.push_back(UnSortedResults[First]);
    retVal.push_back(UnSortedResults[Second]);

    while (Simi.size() > 0)
    {
        First = Second;

        double Max = -1;
        for(Consensus::SortItem item : Simi)
        {
            if (item.x == First || item.y == First)
            {
                if (Max < 0 || item.value > Max)
                {
                    if (item.x == First)
                        Second = item.y;
                    else
                        Second = item.x;
                }
            }
        }

        riter = std::remove_if(Simi.begin(), Simi.end(), [First](Consensus::SortItem r){ return r.x == First || r.y == First; });
        Simi.erase(riter, Simi.end());

        retVal.push_back(UnSortedResults[Second]);
    }

    return retVal;
}

std::vector<std::shared_ptr<Bicluster>> Consensus::CreateGenericBiclusters(std::vector<std::vector<std::shared_ptr<Bicluster>>> orgResults, double percentage, uint minClusterSize, int maxBiclusterNum)
{
    std::vector<std::shared_ptr<Bicluster>> retVal;

    //Are there any results at all?
    if (orgResults.size() == 0)
    {
        throw "No results";
    }
    //Every result on the list should have the same number of bi-clusters
    else if (std::all_of(orgResults.begin(), orgResults.end(), [orgResults](const std::vector<std::shared_ptr<Bicluster>>& in){ return in.size() != orgResults[0].size(); } ))
    {
        throw "Results inconsistent";
    }
    //nothing to do if there is only one result
    else if (orgResults.size() == 1)
    {
        return orgResults[0];
    }

    orgResults = SortResults(orgResults);

    std::vector<Array<int>> Connectors;
    std::vector<Array<double>> Similarities;

    std::vector<std::vector<std::shared_ptr<Bicluster>>> Connected(orgResults[0].size());
    std::vector<std::vector<double>> SimValues(orgResults[0].size());

    ClassicalHungarian MunkresEngine;

    for (uint i = 1; i < orgResults.size(); ++i)
    {
        Array<double> CM = GetCostMatrixForBiclusters(orgResults[i - 1], orgResults[i], Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

        MunkresEngine.SetCostMatrix(CM, ClassicalHungarian::Max);

        MunkresEngine.RunMunkres();
        Array<int> Connector = MunkresEngine.M;
        Array<double> Similarity = MunkresEngine.C_orig;
        Connectors.push_back(Connector);
        Similarities.push_back(Similarity);
    }

    for (uint b = 0; b < orgResults[0].size(); ++b)
    {
        std::vector<int> Indexes(orgResults.size(), -1);
        Indexes[0] = b;

        for (uint i = 1; i < orgResults.size(); ++i)
        {
            for (uint br = 0; br < orgResults[0].size(); ++br)
            {
                if (Connectors[i - 1][Indexes[i - 1]][br] == 1)
                {
                    Indexes[i] = br;

                    SimValues[b].push_back(Similarities[i - 1][Indexes[i - 1]][br]);
                    break;
                }
            }
        }

        for (uint i = 0; i < Indexes.size(); ++i)
        {
            Connected[b].push_back(orgResults[i][Indexes[i]]);
        }
    }

    for (uint i = 0; i < Connected.size(); ++i)
    {
        if (dataMatrix != nullptr)
        {
            for( std::shared_ptr<Bicluster>& item : Connected[i])
            {
                if (item->cluster1.size() > 0 && item->cluster2.size() > 0)
                    item->ACV = dataMatrix->AverageCorrelationValue(item->cluster1, item->cluster2);
                else
                    item->ACV = 0;
            }
        }

        std::vector<ClusterItem> Cluster1;
        std::vector<ClusterItem> Cluster2;

        for ( std::shared_ptr<Bicluster>& item : Connected[i])
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

        if (ExtractType == Consensus::MergeType::Standard)
        {
            double MinWeight = (double)orgResults.size();

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

                if ((FinalCluster1.size() >= minClusterSize) || (MinWeight == 0.1))
                    done = true;
                else
                    MinWeight *= 0.9;
            }

            MinWeight = (int)std::round((double)orgResults.size() * percentage);

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

                if ((FinalCluster2.size() >= minClusterSize) || (MinWeight == 0.1))
                    done = true;
                else
                    MinWeight *= 0.9;
            }

            std::shared_ptr<Bicluster> newBic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);
            retVal.push_back(newBic);
        }
        else if (ExtractType == MergeType::ByACV)
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

            std::shared_ptr<Bicluster> newBic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);
            retVal.push_back(newBic);
        }
        else if (ExtractType == MergeType::ByACVHeuristic)
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

                if (Cluster1.size() == 0 && Cluster2.size() == 0)
                    done = true;
            }

            std::shared_ptr<Bicluster> newBic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2, dataMatrix->AverageCorrelationValue(FinalCluster1, FinalCluster2), nullptr);
            retVal.push_back(newBic);
        }
    }

    if (maxBiclusterNum > 0 && retVal.size() > (uint)maxBiclusterNum)
    {
        std::vector<double> SimValuesList(SimValues.size());

        if (dataMatrix != nullptr)
        {
            for (uint i = 0; i < retVal.size(); ++i)
            {
                if (retVal[i]->cluster1.size() > 0 && retVal[i]->cluster2.size() > 0)
                {
                    std::shared_ptr<double> tmp = dataMatrix->AverageCorrelationValue(retVal[i]->cluster1, retVal[i]->cluster2);
                    SimValuesList[i] = *tmp;
                }
                else
                    SimValuesList[i] = 0;
            }
        }
        else
        {
            std::transform(SimValues.begin(), SimValues.end(), SimValuesList.begin(), [](const std::vector<double>& c){ return (std::accumulate(c.begin(), c.end(), 0.0) / (double)c.size()); });
        }

        while (retVal.size() > (uint)maxBiclusterNum)
        {
            double MinSim = SimValuesList[0];
            int IndexToRemove = 0;

            for (uint i = 0; i < SimValuesList.size(); ++i)
            {
                if (SimValuesList[i] > MinSim)
                {
                    MinSim = SimValuesList[i];
                    IndexToRemove = i;
                }
            }

            retVal.erase(retVal.begin() + IndexToRemove);
            SimValuesList.erase(SimValuesList.begin() + IndexToRemove);
        }
    }

    return retVal;
}


