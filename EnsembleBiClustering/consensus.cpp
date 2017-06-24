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
    uint bicSize = orgResults[0].size();

    //Are there any results at all?
    if (orgResults.size() == 0)
    {
        throw "No results";
    }
    //Every result on the list should have the same number of bi-clusters
    else if (!std::all_of(orgResults.begin(), orgResults.end(), [bicSize](const std::vector<std::shared_ptr<Bicluster>>& in){ return in.size() == bicSize; } ))
    {
        if (expectedBiClusterCount > 0)
        {
            for(int r = 0; r < orgResults.size(); ++r)
            {
                if (orgResults[r].size() < expectedBiClusterCount)
                {
                    std::shared_ptr<Bicluster> emptyBic = std::make_shared<Bicluster>(-1, std::vector<int>(), std::vector<int>(), nullptr, nullptr);

                    while (orgResults[r].size() < expectedBiClusterCount)
                    {
                        orgResults[r].push_back(emptyBic);
                    }
                }
                else if (orgResults[r].size() > expectedBiClusterCount)
                {
                    Array<double> CM = GetCostMatrixForBiclusters(orgResults[r], orgResults[r], Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

                    while(orgResults[r].size() > expectedBiClusterCount)
                    {
                        int remove = 0;
                        int merge = 1;
                        double maxSimi = CM[remove][merge];

                        for(int i = 0; i < CM.height; ++i)
                        {
                            for(int j = 0; j < CM.width; ++j)
                            {
                                if (i != j)
                                {
                                    if (CM[i][j] > maxSimi)
                                    {
                                        remove = i;
                                        merge = j;
                                        maxSimi = CM[remove][merge];
                                    }
                                }
                            }
                        }

                        for(int c1 : orgResults[r][remove]->cluster1)
                        {
                            auto it = std::find_if(orgResults[r][remove]->cluster1.begin(), orgResults[r][remove]->cluster1.end(), [c1](int iter) { return iter == c1; } );

                            if (it == orgResults[r][remove]->cluster1.end())
                                orgResults[r][merge]->cluster1.push_back(c1);
                        }

                        for(int c2 : orgResults[r][remove]->cluster2)
                        {
                            auto it = std::find_if(orgResults[r][remove]->cluster2.begin(), orgResults[r][remove]->cluster2.end(), [c2](int iter) { return iter == c2; } );

                            if (it == orgResults[r][remove]->cluster2.end())
                                orgResults[r][merge]->cluster2.push_back(c2);
                        }

                        orgResults[r].erase(orgResults[r].begin() + remove);

                        Array<double> cmUpdate(orgResults[r].size(), orgResults[r].size());

                        for(int i = 0; i < CM.height; ++i)
                        {
                            for(int j = 0; j < CM.width; ++j)
                            {
                                int iOut = i;
                                int jOut = j;

                                if (i == remove || j == remove)
                                    continue;

                                if (i > remove)
                                    iOut--;

                                if (j > remove)
                                    jOut--;

                                if (j == merge || i == merge)
                                {
                                    //cmUpdate[iOut][jOut] = orgResults[r][iOut]->Compare(orgResults[r][jOut], Enums::SimilarityMethods::JaccardIndex);
                                    CostMatrixWorker *st = new CostMatrixWorker(&cmUpdate[iOut][jOut], orgResults[r][iOut], orgResults[r][jOut], Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

                                    QThreadPool::globalInstance()->start(st);
                                }
                                else
                                {
                                    cmUpdate[iOut][jOut] = CM[i][j];
                                }
                            }
                        }

                        QThreadPool::globalInstance()->waitForDone();

                        CM.Initialize(cmUpdate);

                        //Array<double> test = GetCostMatrixForBiclusters(orgResults[r], orgResults[r], Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

                        //qDebug() << test[10, 10];
                    }
                }
            }
        }
        else
        {
            throw "Results inconsistent";
        }
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

    std::vector<std::shared_ptr<Bicluster>> retVal(Connected.size());

    for (uint i = 0; i < Connected.size(); ++i)
    {
        retVal[i] = std::make_shared<Bicluster>(0, 0, 0, 0);

        ConsensusWorker *st = new ConsensusWorker(retVal[i], dataMatrix, Connected[i], ExtractType, (double)orgResults.size(), minClusterSize, percentage);

        QThreadPool::globalInstance()->start(st);


    }

    QThreadPool::globalInstance()->waitForDone();

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



