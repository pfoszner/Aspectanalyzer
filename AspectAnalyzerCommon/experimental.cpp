#include "experimental.h"

void Experimental::TestTriclustering()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, 1, Enums::Methods::PLSA, -1);

    std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1);

    newObject->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

    newObject->SetEnsemble(test);

    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

    params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

    newObject->Compute(params);
}

void Experimental::RunTriclustering()
{
    std::vector<Enums::Methods> methods;

    methods.push_back(Enums::Methods::PLSA);
    methods.push_back(Enums::Methods::LEAST_SQUARE_ERROR);
    methods.push_back(Enums::Methods::KULLBACK_LIEBER);
    methods.push_back(Enums::Methods::NonSmooth_KULLBACK_LIEBER);

    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(Enums::Methods method : methods)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> single  = engine->db->GetResults(-1, 1, method, -1);

        double bestValue = -1;
        int best = 0;

        for(uint m = 0; m < single.size(); ++m)
        {
            double value = 0;
            for(std::shared_ptr<Bicluster> bic : single[m]->foundedBiclusters)
            {
                value += *bic->ACV;
            }
            value /= single[m]->foundedBiclusters.size();

            if (value > bestValue)
            {
                bestValue = value;
                best = m;
            }
        }

        std::shared_ptr<NMF> tmpPtr = std::static_pointer_cast<NMF>(single[best]);

        tmpPtr->RebuildBiclusters();

        //single[best]->PostProcessingTask();

        test.push_back(single[best]);
    }

    std::vector<Consensus::MergeType> mt;

    //mt.push_back(Consensus::MergeType::ByACV);
    //mt.push_back(Consensus::MergeType::ByACVHeuristic);
    //mt.push_back(Consensus::MergeType::Standard);

    //for(Consensus::MergeType imt : mt)
    //{
        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1);

        newObject->expectedBiClusterCount = 8;

        newObject->dataMatrix->expectedBiClusterCount = 8;

        newObject->SetEnsemble(test);

        //newObject->ExtractType = imt;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);

        //engine->AddBiClusteringTask(newObject);
    //}
       //engine->ServeQueue();
}

void Experimental::RunConsensus()
{
    std::vector<Enums::Methods> methods;

    methods.push_back(Enums::Methods::PLSA);
    methods.push_back(Enums::Methods::LEAST_SQUARE_ERROR);
    methods.push_back(Enums::Methods::KULLBACK_LIEBER);
    methods.push_back(Enums::Methods::NonSmooth_KULLBACK_LIEBER);

    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(Enums::Methods method : methods)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> single  = engine->db->GetResults(-1, 1, method, -1);

        double bestValue = -1;
        int best = 0;

        for(uint m = 0; m < single.size(); ++m)
        {
            double value = 0;
            for(std::shared_ptr<Bicluster> bic : single[m]->foundedBiclusters)
            {
                value += *bic->ACV;
            }
            value /= single[m]->foundedBiclusters.size();

            if (value > bestValue)
            {
                bestValue = value;
                best = m;
            }
        }

        test.push_back(single[best]);
    }

    std::vector<Consensus::MergeType> mt;

    mt.push_back(Consensus::MergeType::ByACV);
    mt.push_back(Consensus::MergeType::ByACVHeuristic);
    mt.push_back(Consensus::MergeType::Standard);

    for(Consensus::MergeType imt : mt)
    {
        std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1);

        newObject->expectedBiClusterCount = 8;

        newObject->dataMatrix->expectedBiClusterCount = 8;

        newObject->SetEnsemble(test);

        newObject->ExtractType = imt;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        //auto res = newObject->Compute(params);

        //engine->db->SaveResult(res);

        engine->AddBiClusteringTask(newObject);
    }

    //engine->ServeQueue();
}
