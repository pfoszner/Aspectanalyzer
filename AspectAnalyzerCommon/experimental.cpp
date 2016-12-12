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

void Experimental::RunConsensus()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, 1, -1, -1);

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

        engine->AddBiClusteringTask(newObject);
    }
}
