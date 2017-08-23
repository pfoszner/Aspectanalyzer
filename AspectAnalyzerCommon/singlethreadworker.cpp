#include "singlethreadworker.h"

SingleThreadWorker::SingleThreadWorker(std::shared_ptr<BiclusteringObject> task) : task(task)
{
    //this->s = s;
}

void SingleThreadWorker::run()
{
    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

    //params.emplace_back(Enums::Theta, std::make_shared<double>(0.5));

    //params.emplace_back(Enums::ExMethod, std::make_shared<Enums::ExtractingMethod>(Enums::ExtractingMethod::Average));

    //params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(task->dataMatrix->expectedBiClusterCount));

    //params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(3));

    std::shared_ptr<BiclusteringObject> jobDone = task->Compute(params);

    emit ReportDone(jobDone);
}
