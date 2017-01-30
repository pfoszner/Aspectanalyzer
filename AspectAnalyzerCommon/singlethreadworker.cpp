#include "singlethreadworker.h"

SingleThreadWorker::SingleThreadWorker(std::queue<std::shared_ptr<BiclusteringObject>>& tmpQueue, std::shared_ptr<BiclusteringObject> task, int s) : tmpQueue(tmpQueue), task(task), s(s)
{
    //this->tmpQueue = tmpQueue;
    //this->task = task;
    //this->s = s;
}

void SingleThreadWorker::run()
{
    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

    params.emplace_back(Enums::Theta, std::make_shared<double>(0.5));

    params.emplace_back(Enums::ExMethod, std::make_shared<Enums::ExtractingMethod>(Enums::ExtractingMethod::Average));

    //params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(task->dataMatrix->expectedBiClusterCount));

    params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(3));

    std::shared_ptr<BiclusteringObject> test = task->Compute(params);

    m.lock();

    tmpQueue.push(test);

    qDebug() << s << " Items left";

    m.unlock();
}
