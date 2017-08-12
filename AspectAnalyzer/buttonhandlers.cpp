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

    //QWizard wizard;
    //wizard.addPage(createIntroPage());
    //wizard.addPage(createRegistrationPage());
    //wizard.addPage(createConclusionPage());

    //wizard.setWindowTitle("Trivial Wizard");
    //wizard.show();
    //wizard.exec();

    AddBiclusteringTask wizard(engine);

    wizard.exec();

    setTasksLabels(QString::number(engine->GetRunning()), QString::number(engine->GetInQueue()));

//    std::shared_ptr<BiclusteringObject> newObject;

//    switch(methodID)
//    {
//        case Enums::PLSA:
//            newObject = std::make_shared<PLSA>(engine->CurrentVmatrix);
//            break;
//        case Enums::LEAST_SQUARE_ERROR:
//            newObject = std::make_shared<LSE>(engine->CurrentVmatrix);
//            break;
//        case Enums::KULLBACK_LIEBER:
//            newObject = std::make_shared<KullbackLeibler>(engine->CurrentVmatrix);
//            break;
//        case Enums::NonSmooth_KULLBACK_LIEBER:
//            newObject = std::make_shared<nsKullbackLeibler>(engine->CurrentVmatrix, 0.5);
//            break;
//    }

//    engine->AddBiClusteringTask(newObject);
//    setTasksLabels(QString::number(engine->GetRunning()), QString::number(engine->GetInQueue()));
}

//void ButtonHandlers::btnLoadFromFileSlot()
//{
//    QString fileName = QFileDialog::getOpenFileName(nullptr,
//        tr("Open Image"), "", tr("AspectAnalyzer Files (*.vmatrix *.arff)"));

//    //QString fileName = "../Data/11.vmatrix";

//    QFileInfo fileInfo(fileName);

//    engine->LoadDataMatrix(fileName);

//    double test1 = arma::mean(engine->CurrentVmatrix->data.elem(engine->CurrentVmatrix->data > 0));
//    auto test2 = arma::median(engine->CurrentVmatrix->data.elem(engine->CurrentVmatrix->data > 0));

//    qDebug() << "Mean: " << test1 << " Median: " << test2;

//    QString idLabel = "Matrix not in DB";

//    if (engine->CurrentVmatrix->idMatrix != nullptr && *engine->CurrentVmatrix->idMatrix > 0)
//    {
//        idLabel = QString::number(*engine->CurrentVmatrix->idMatrix);
//    }

//    emit setMatrixLabels(engine->CurrentVmatrix->name,
//                         QString::number(engine->CurrentVmatrix->data.n_rows),
//                         QString::number(engine->CurrentVmatrix->data.n_cols),
//                         QString::number(engine->CurrentVmatrix->data.min()),
//                         QString::number(engine->CurrentVmatrix->data.max()),
//                         idLabel);
//}

//void ButtonHandlers::btnLoadFromDatabaseSlot()
//{
//    for(uint i = 1; i <= 216; ++i)
//    {
//        QString fileName = "../Data/" + QString::number(i) + ".vmatrix";

//        engine->LoadDataMatrix(fileName);

//        //engine->CurrentVmatrix = engine->db->GetMatrix(1);

//        QString idLabel = "Matrix not in DB";

//        if (engine->CurrentVmatrix->idMatrix != nullptr && *engine->CurrentVmatrix->idMatrix > 0)
//        {
//            idLabel = QString::number(*engine->CurrentVmatrix->idMatrix);
//        }

//        emit setMatrixLabels(engine->CurrentVmatrix->name,
//                             QString::number(engine->CurrentVmatrix->data.n_rows),
//                             QString::number(engine->CurrentVmatrix->data.n_cols),
//                             QString::number(engine->CurrentVmatrix->data.min()),
//                             QString::number(engine->CurrentVmatrix->data.max()),
//                             idLabel);

//        for(uint r = 0; r < 10; ++r)
//        {
//            for(int m = 0; m <= 3; ++m)
//            {
//                btnAddTaskSlot(m);
//                if (false)//(engine->CurrentVmatrix->idMatrix < 0)
//                {
//                    *engine->CurrentVmatrix->idMatrix = engine->db->SaveMatrix(engine->CurrentVmatrix->data, engine->CurrentVmatrix->name, engine->CurrentVmatrix->group, Enums::MatrixType::V, -1);
//                    engine->db->SaveBiclusters(engine->CurrentVmatrix->expectedBiClusters, *engine->CurrentVmatrix->idMatrix, -1);
//                    engine->db->SaveLabels(engine->CurrentVmatrix->rowLabels, *engine->CurrentVmatrix->idMatrix);
//                    engine->db->SaveLabels(engine->CurrentVmatrix->columnLabels, *engine->CurrentVmatrix->idMatrix);
//                }
//            }
//        }
//    }
//    qDebug() << "Data loaded sucessfully";
//}



//void ButtonHandlers::btnTuneSlot()
//{
//    //QWizard wizard;
//    //wizard.addPage(createIntroPage());
//    //wizard.addPage(createRegistrationPage());
//    //wizard.addPage(createConclusionPage());

//    //wizard.setWindowTitle("Trivial Wizard");
//    //wizard.show();
//    //wizard.exec();

//    AddBiclusteringTask wizard;

//    wizard.show();
//    wizard.exec();

//    //exper->LoadKumalBiclusters();

//    //exper->RunAllConsensus();

//    //exper->RunAllTriclustering();

//    //qDebug() << "Mission Acomplished";

//    //return;

//    //for(int i=0; i<=3; ++i)
//    //{
//    //    for(int r=0; r < 10; ++r)
//    //    {
//    //        btnAddTaskSlot(i);
//    //    }
//    //}

//    //engine->ServeQueue();

//    //exper->RunConsensus();

//    //exper->RunTriclustering();

//    //exper->CompareGrandTruthMiRNA();

//    //exper->ARFFPlay();

//    //exper->StartCustom();


//}

void ButtonHandlers::btnCustomSlot(QString mode)
{
    qDebug() << "Starting custom calculation in mode = " << mode;

    exper->StartCustom();

}

void ButtonHandlers::btnPlaySlot()
{
    engine->ServeQueue();
}

void ButtonHandlers::btnPauseSlot()
{

}

void ButtonHandlers::btnStopSlot()
{

}
