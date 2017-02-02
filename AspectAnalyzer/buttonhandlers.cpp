#include "buttonhandlers.h"


//Ctr
ButtonHandlers::ButtonHandlers(QObject *parent) :
    QObject(parent)
{
    engine = std::make_shared<ComputingEngine>();

    exper = std::make_shared<Experimental>(engine);
}

ButtonHandlers::~ButtonHandlers()
{
}

//Add new bi-clustering task to worker
void ButtonHandlers::btnAddTaskSlot(int methodID)
{
    std::shared_ptr<BiclusteringObject> newObject;

    switch(methodID)
    {
        case Enums::PLSA:
            newObject = std::make_shared<PLSA>(engine->CurrentVmatrix);
            break;
        case Enums::LEAST_SQUARE_ERROR:
            newObject = std::make_shared<LSE>(engine->CurrentVmatrix);
            break;
        case Enums::KULLBACK_LIEBER:
            newObject = std::make_shared<KullbackLeibler>(engine->CurrentVmatrix);
            break;
        case Enums::NonSmooth_KULLBACK_LIEBER:
            newObject = std::make_shared<nsKullbackLeibler>(engine->CurrentVmatrix, 0.5);
            break;
    }

    engine->AddBiClusteringTask(newObject);
    setTasksLabels(QString::number(engine->GetRunning()), QString::number(engine->GetInQueue()));
}

void ButtonHandlers::btnLoadFromFileSlot()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,
        tr("Open Image"), "", tr("AspectAnalyzer Files (*.vmatrix *.arff)"));

    //QString fileName = "../Data/11.vmatrix";

    QFileInfo fileInfo(fileName);

    engine->LoadDataMatrix(fileName);

    QString idLabel = "Matrix not in DB";

    if (engine->CurrentVmatrix->idMatrix != nullptr && *engine->CurrentVmatrix->idMatrix > 0)
    {
        idLabel = QString::number(*engine->CurrentVmatrix->idMatrix);
    }

    emit setMatrixLabels(engine->CurrentVmatrix->name,
                         QString::number(engine->CurrentVmatrix->data.n_rows),
                         QString::number(engine->CurrentVmatrix->data.n_cols),
                         QString::number(engine->CurrentVmatrix->data.min()),
                         QString::number(engine->CurrentVmatrix->data.max()),
                         idLabel);
}

void ButtonHandlers::btnLoadFromDatabaseSlot()
{
    for(uint i = 1; i <= 216; ++i)
    {
        QString fileName = "../Data/" + QString::number(i) + ".vmatrix";

        engine->LoadDataMatrix(fileName);

        //engine->CurrentVmatrix = engine->db->GetMatrix(1);

        QString idLabel = "Matrix not in DB";

        if (engine->CurrentVmatrix->idMatrix != nullptr && *engine->CurrentVmatrix->idMatrix > 0)
        {
            idLabel = QString::number(*engine->CurrentVmatrix->idMatrix);
        }

        emit setMatrixLabels(engine->CurrentVmatrix->name,
                             QString::number(engine->CurrentVmatrix->data.n_rows),
                             QString::number(engine->CurrentVmatrix->data.n_cols),
                             QString::number(engine->CurrentVmatrix->data.min()),
                             QString::number(engine->CurrentVmatrix->data.max()),
                             idLabel);

        for(uint r = 0; r < 10; ++r)
        {
            for(int m = 0; m <= 3; ++m)
            {
                btnAddTaskSlot(m);
                if (false)//(engine->CurrentVmatrix->idMatrix < 0)
                {
                    *engine->CurrentVmatrix->idMatrix = engine->db->SaveMatrix(engine->CurrentVmatrix->data, engine->CurrentVmatrix->name, engine->CurrentVmatrix->group, Enums::MatrixType::V, -1);
                    engine->db->SaveBiclusters(engine->CurrentVmatrix->expectedBiClusters, *engine->CurrentVmatrix->idMatrix, -1);
                    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, *engine->CurrentVmatrix->idMatrix);
                    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, *engine->CurrentVmatrix->idMatrix);
                }
            }
        }
    }
    qDebug() << "Data loaded sucessfully";
}

void ButtonHandlers::btnTuneSlot()
{
    //for(int i=0; i<=3; ++i)
    //{
    //    for(int r=0; r < 1; ++r)
    //    {
    //        btnAddTaskSlot(i);
    //    }
    //}

    //engine->ServeQueue();

    //exper->RunConsensus();

    //exper->RunTriclustering();

    //exper->CompareGrandTruthMiRNA();

    exper->ARFFPlay();

    qDebug() << "Mission Acomplished";
}

void ButtonHandlers::btnPlaySlot()
{
    engine->ServeQueue();
}

void ButtonHandlers::btnPauseSlot()
{
    engine->LoadDataMatrix("/mnt/E/TCGA/DataMatrixTCGAThyroid carcinoma.vmatrix");

    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, 1);
    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, 1);

    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, 4);
    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, 4);

    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, 7);
    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, 7);

    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, 10);
    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, 10);
}

void ButtonHandlers::btnStopSlot()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, -1, -1, -1);

    for(std::shared_ptr<BiclusteringObject> result : test)
    {

        if (result->idMethod < 8)
            continue;

        QFile retVal("result" + QString::number(result->idMethod) + "_"+QString::number(result->idResult)+".txt");

        retVal.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&retVal);

        int index = 0;

        for(std::shared_ptr<Bicluster> bic : result->foundedBiclusters)
        {
            out << "Bicluster " << ++index << ". Average corelation value: " << *bic->ACV << "\n";

            out << "Cluster1:\n";

            for(int c1 : bic->cluster1)
            {
                out << result->dataMatrix->rowLabels[c1].value << "\n";
                //out << c1 << "\n";
            }

            out << "\nCluster2:\n";

            for(int c2 : bic->cluster2)
            {
                out << result->dataMatrix->columnLabels[c2].value << "\n";
                //out << c2 << "\n";
            }

            out << "\n\n";
        }

        retVal.close();

    }

    QFile labels("genes.txt");

    labels.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream outL(&labels);

    for(Label item : test[0]->dataMatrix->columnLabels)
    {
        outL << item.value << "\n";
    }

    labels.close();
}
