#include "ensembletask.h"

void EnsembleTask::SetEnsemble(const std::vector<std::shared_ptr<BiclusteringObject>>& Ensemble)
{
    if (Ensemble.size() > 0)
    {
        tasksToEnsemble.clear();

        for(auto item : Ensemble)
        {
            tasksToEnsemble.push_back(item);
        }
    }
}

std::shared_ptr<BiclusteringObject> EnsembleTask::Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    std::shared_ptr<BiclusteringObject> null;

    params.clear();

    return null;
}

void EnsembleTask::ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    for (auto &param : params)
    {
        switch (std::get<0>(param))
        {
            case Enums::NumberOfBiClusters:
            {
                int* m = reinterpret_cast<int*>(std::get<1>(param).get());
                if (m != nullptr) {
                    expectedBiClusterCount = *m;
                }
                break;
            }
            case Enums::Multithreading:
            {
                bool* m = reinterpret_cast<bool*>(std::get<1>(param).get());
                if (m != nullptr) {
                    multithreading = *m;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

std::vector<std::vector<std::shared_ptr<Bicluster>>> EnsembleTask::GetBiclustersListGruppedByTask()
{
    std::vector<std::vector<std::shared_ptr<Bicluster>>> retVal;

    if (tasksToEnsemble.size() > 0)
    {
        for (std::shared_ptr<BiclusteringObject> item : tasksToEnsemble)
        {
            if (item->foundedBiclusters.size() > 0)
            {
                std::vector<std::shared_ptr<Bicluster>> newItem;

                for (std::shared_ptr<Bicluster> bic : item->foundedBiclusters)
                {
                    newItem.emplace_back(bic);
                }

                retVal.emplace_back(newItem);
            }
        }
    }

    return retVal;
}

std::vector<std::shared_ptr<Bicluster>> EnsembleTask::GetBiclustersList()
{
    std::vector<std::shared_ptr<Bicluster>> retVal;

    if (tasksToEnsemble.size() > 0)
    {
        for (std::shared_ptr<BiclusteringObject> item : tasksToEnsemble)
        {
            if (item->foundedBiclusters.size() > 0)
            {
                for (std::shared_ptr<Bicluster> bic : item->foundedBiclusters)
                {
                    retVal.emplace_back(bic);
                }
            }
        }
    }

    return retVal;
}
