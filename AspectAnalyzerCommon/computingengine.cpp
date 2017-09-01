#include "computingengine.h"


ComputingEngine::ComputingEngine(QObject *parent) : QObject(parent)
{
    this->runningTasks = 0;
    this->db = std::make_shared<DBTools>("database.db");
    qRegisterMetaType<ResultPointer>("ResultPointer");
}

void ComputingEngine::receiveData(QByteArray data)
{
    qDebug() << data.length();

    if (data.length() > 10)
    {
        std::shared_ptr<BiclusteringObject> task = std::make_shared<BiclusteringObject>(data);

        task->dataMatrix = db->GetMatrix(task->idMatrix);

        if (task->mode == BiclusteringObject::ComputingMode::RemoteToCompute)
        {
            AddBiClusteringTask(task);
        }
        else if (task->mode == BiclusteringObject::ComputingMode::RemoteDone)
        {
            lock.lock();
            resultsToWrite.push(task);
            lock.unlock();
            CheckResultsToWrite();
        }
    }
    else if (data.length() == 4)
    {
        QByteArray source = data.mid(0, 4);
        qint32 command;
        QDataStream data(&source, QIODevice::ReadWrite);
        data >> command;

        if (command == CommandType::StartQueue)
        {
            ServeQueue();
        }
    }
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

void ComputingEngine::AddBiClusteringTask(ResultPointer task)
{
    connect(task.get(), SIGNAL(ReportProgress(int)), this, SLOT(UpdateProgress(int)));

    queue.push(task);

    setTasksLabels(QString::number(GetRunning()), QString::number(GetInQueue()));
}

void ComputingEngine::CheckResultsToWrite()
{
    lock.lock();
    uint queueSize = resultsToWrite.size();
    lock.unlock();

    if (queueSize > 0)
    {
        while (queueSize) {

            lock.lock();

            ResultPointer taskToSave = resultsToWrite.front();

            resultsToWrite.pop();

            db->SaveResult(taskToSave);

            queueSize = resultsToWrite.size();

            lock.unlock();
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

void ComputingEngine::UpdateProgress(int value)
{
    lock.lock();

    currentProgressSteps += value;

    int progresValue = std::trunc(((double)currentProgressSteps / (double)progressSteps)*100);

    if (progresValue < 100)
    {
        double elapsed = time(0) - queueStart;

        double togo = ((progressSteps - currentProgressSteps)*elapsed)/currentProgressSteps;

        setProgressChange(progresValue, GetHumanTime(togo));

        lock.unlock();
    }
    else
    {
        setProgressChange(progresValue, "Done");
        lock.unlock();
        CheckResultsToWrite();

        if (queue.size() > 0)
            ServeQueue();
    }
}

void ComputingEngine::CheckWriteResult(ResultPointer jobDone)
{
    lock.lock();

    if (taskToComputute > 0)
        taskToComputute--;
    else
        runningTasks--;

    setTasksLabels(QString::number(GetRunning()), QString::number(GetInQueue()));

    resultsToWrite.push(jobDone);
    int count = resultsToWrite.size();
    lock.unlock();

    if (count >= 1)
        CheckResultsToWrite();

    qDebug() << "Result done";
}

void ComputingEngine::ServeQueue()
{
    setProgressChange(0, "-");

    QThreadPool::globalInstance()->setMaxThreadCount(maxThreadAllowd);

    qDebug() << "Thread Count" << QThreadPool::globalInstance()->maxThreadCount() << " Current Time: " << time(0);

    taskToComputute = queue.size();

    progressSteps = taskToComputute * 100;

    queueStart = time(0);

    int index = -1;

    while ( queue.size() > 0)
    {
        currentProgressSteps = 0;

        ResultPointer task = queue.front();

        queue.pop();

        if (index++ < 0)
        {
            SingleThreadWorker *st = new SingleThreadWorker(task);

            connect(st, SIGNAL(ReportDone(ResultPointer)), this, SLOT(CheckWriteResult(ResultPointer)), Qt::ConnectionType::QueuedConnection);

            QThreadPool::globalInstance()->start(st);
        }
        else
        {
            bool connected = aaclient.connectToHost(slaves[index]);

            if (connected)
            {
                aaclient.writeData(task->Serialize());
                aaclient.disconnectFromHost();
            }
        }

        if (index >= slaves.size())
        {
            index = -1;
        }
    }

    for(QString slave : slaves)
    {
        bool connected = aaclient.connectToHost(slave);

        if (connected)
        {
            QByteArray temp;
            QDataStream data(&temp, QIODevice::ReadWrite);
            data << (qint32)CommandType::StartQueue;

            aaclient.writeData(temp);
            aaclient.disconnectFromHost();
        }
    }

    if (QThreadPool::globalInstance()->maxThreadCount() > taskToComputute)
    {
        runningTasks = taskToComputute;
    }
    else
    {
        runningTasks = QThreadPool::globalInstance()->maxThreadCount();
        taskToComputute = taskToComputute - runningTasks;
    }

    setTasksLabels(QString::number(GetRunning()), QString::number(GetInQueue()));

    //QThreadPool::globalInstance()->waitForDone();

    //CheckResultsToWrite();

    //setProgressChange(100);

    qDebug() << "Serve Queue Done!";
}


int ComputingEngine::GetInQueue()
{
    if (taskToComputute > 0)
        return taskToComputute;
    else
        return queue.size();
}

int ComputingEngine::GetRunning()
{
    return runningTasks;
}
