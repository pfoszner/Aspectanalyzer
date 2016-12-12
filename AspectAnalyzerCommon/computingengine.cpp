#include "computingengine.h"


ComputingEngine::ComputingEngine()
{
    this->runningTasks = 0;
    this->db = std::make_shared<DBTools>("database.db");
}

void ComputingEngine::LoadDataMatrix(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);

        std::vector <QString> vec;

        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();

            vec.push_back(line);
        }

        this->CurrentVmatrix = std::make_shared<Matrix>(vec);

        file.close();
    }
}

void ComputingEngine::AddBiClusteringTask(std::shared_ptr<BiclusteringObject> task)
{
    queue.push(task);
}

void ComputingEngine::CheckResultsToWrite()
{
    if (resultsToWrite.size() > 0)
    {
        uint queueSize = resultsToWrite.size();

        while (queueSize) {

            std::shared_ptr<BiclusteringObject> taskToSave = resultsToWrite.front();
            resultsToWrite.pop();

            db->SaveResult(taskToSave);

            queueSize = resultsToWrite.size();
        }
    }
}

QString ComputingEngine::GetHumanTime(double time)
{
    QString retVal = "";

    if (time > 60 * 60)
    {
        double hours = std::trunc(time / (60.0 * 60.0));
        time -= hours * 60.0 * 60.0;
        retVal += QString::number(hours) + "h ";
    }

    if (time > 60)
    {
        double minutes = std::trunc(time / 60.0);
        time -= minutes * 60.0;
        retVal += QString::number(minutes) + "min ";
    }

    retVal += QString::number(std::round(time)) + "sec";

    return retVal;
}

void ComputingEngine::ServeQueue()
{
    qDebug() << "Thread Count" << QThreadPool::globalInstance()->maxThreadCount();

    while ( queue.size() > 0 )
    {
        std::shared_ptr<BiclusteringObject> task = queue.front();

        queue.pop();

//        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

//        params.emplace_back(Enums::Theta, std::make_shared<double>(0.5));

//        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(task->data[0]->dataMatrix->expectedBiClusterCount));

        auto& tmpQueue = resultsToWrite;

        int s = queue.size();

        SingleThreadWorker *st = new SingleThreadWorker(tmpQueue, task, s);

        QThreadPool::globalInstance()->start(st);

//        std::shared_ptr<BiclusteringObject> test = task->data[0]->Compute(params);

//        times.push_back(test->time_spent);

//        double averageTime = (std::accumulate(times.begin(), times.end(), 0.0) / times.size()) * queue.size();

//        QString HumanTime = GetHumanTime(averageTime);

//        qDebug() << queue.size() << " Items left (" << test->time_spent << " sec). Time left: " << HumanTime << "sec";

//        resultsToWrite.push(test);
    }

    QThreadPool::globalInstance()->waitForDone();

    CheckResultsToWrite();

    qDebug() << "Serve Queue Done!";

}


int ComputingEngine::GetInQueue()
{
    return queue.size();
}

int ComputingEngine::GetRunning()
{
    return runningTasks;
}
