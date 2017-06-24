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

void Experimental::ARFFPlay()
{
    std::shared_ptr<Matrix> dupa = std::make_shared<Matrix>("/mnt/E/TCGA/DataMatrixTCGA5_mutect2v2.vmatrix");

    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, -1, -1, -1);

    for(uint i = 0; i < dupa->data.n_rows; ++i)
    {
        if (i < 505)
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Prostate");
        else if (i > 1008)
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Head and Neck");
        else
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Thyroid");
    }

    bool first = true;

    for(std::shared_ptr<BiclusteringObject> bitem : test)
    {
        bitem->dataMatrix = dupa;

        if (first)
            bitem->GenerateARFFFile("/mnt/E/TCGA/arff/test.arff", 1);

        std::vector<int> indexes;

        for(std::shared_ptr<Bicluster> bic : bitem->foundedBiclusters)
        {
            for(int b : bic->cluster2)
            {
                auto iter = std::find_if(indexes.begin(), indexes.end(), [b](int s){ return s == b; });

                if (iter == indexes.end())
                    indexes.push_back(b);
            }
        }

        bitem->GenerateARFFFile("/mnt/E/TCGA/arff/test_single" + QString::number(bitem->idResult) + ".arff", 1, indexes);

        std::vector <QString> gs = GetSignature();

        std::vector<int> indexes2;

        for(QString item : gs)
        {
            for(Label l : dupa->columnLabels)
            {
                if (l.value == item)
                {
                    int in = l.indexNbr;
                    auto iter = std::find_if(indexes2.begin(), indexes2.end(), [in](int s){ return s == in; });

                    if (iter == indexes2.end())
                        indexes2.push_back(in);
                }
            }
        }

        if (first)
        {
            bitem->GenerateARFFFile("/mnt/E/TCGA/arff/test_rafal.arff", 1, indexes2);
            first = false;
        }

        std::vector<int> indexes3;

        for(int it : indexes)
        {
            auto iter = std::find_if(indexes3.begin(), indexes3.end(), [it](int s){ return s == it; });

            if (iter == indexes3.end())
                indexes3.push_back(it);
        }

        for(int it : indexes2)
        {
            auto iter2 = std::find_if(indexes3.begin(), indexes3.end(), [it](int s){ return s == it; });

            if (iter2 == indexes3.end())
                indexes3.push_back(it);
        }

        bitem->GenerateARFFFile("/mnt/E/TCGA/arff/test_super" + QString::number(bitem->idResult) + ".arff", 1, indexes3);

        std::vector<int> indexes4;

        for(int it : indexes)
        {
            auto iter2 = std::find_if(indexes2.begin(), indexes2.end(), [it](int s){ return s == it; });

            if (iter2 != indexes2.end())
                indexes4.push_back(it);
        }

        bitem->GenerateARFFFile("/mnt/E/TCGA/arff/test_join" + QString::number(bitem->idResult) + ".arff", 1, indexes4);
    }
}

std::vector <QString> Experimental::GetSignature()
{
    std::vector <QString> vector;

    QFile file("/mnt/E/TCGA/genes.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);



        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();

            vector.push_back(line);
        }
    }

    return vector;
}

void Experimental::CompareGrandTruthMiRNA()
{
    std::vector<std::shared_ptr<BiclusteringObject>> AllResults  = engine->db->GetResults(-1, -1, -1, -1);

    std::vector<std::shared_ptr<Bicluster>> expected;

    std::vector<std::tuple<QString, QString>> classes;

    QFile textFile("/mnt/D/AppData/owncloud/Praca/Publikacje/2016/Elsevier/daneWojtka/class.csv");

    textFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream textStream(&textFile);

    while (true)
    {
        QString line = textStream.readLine();
        if (line.isNull())
            break;
        else
        {
            QStringList tmp = line.split(';');
            classes.emplace_back(tmp[0].trimmed(), tmp[1].trimmed());
        }
    }

    std::vector<int> cluster1;
    cluster1.push_back(0);
    cluster1.push_back(1);
    cluster1.push_back(2);

    double singleAvg = 0;

    bool header = true;

    for(std::shared_ptr<BiclusteringObject> res : AllResults)
    {
        if (header)
        {
            std::vector<std::vector<int>> indexes;
            std::vector<QString> dicLabel;

            for(Label label : res->dataMatrix->columnLabels)
            {
                QString currClass;

                for(uint c = 0; c < classes.size(); ++c)
                {
                    if (std::get<0>(classes[c]) == label.value)
                    {
                        currClass = std::get<1>(classes[c]);
                        break;
                    }
                }

                bool added = false;

                for(uint b = 0; b < dicLabel.size(); ++b)
                {
                    if (dicLabel[b] == currClass)
                    {
                        indexes[b].push_back(label.indexNbr);
                        added = true;
                        break;
                    }
                }

                if (!added)
                {
                    dicLabel.push_back(currClass);
                    indexes.push_back(std::vector<int>(0));
                    indexes[dicLabel.size() - 1].push_back(label.indexNbr);
                }

            }

            for (std::vector<int> single : indexes)
            {
                std::shared_ptr<double> ACV;
                std::shared_ptr<double> Similarity;
                std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, single, ACV, Similarity);
                expected.push_back(NewBic);
            }
            header = false;
        }

        Array<double> CM = res->GetCostMatrixForBiclusters(expected, res->foundedBiclusters, Enums::BiclusterCompareMode::Second, Enums::SimilarityMethods::JaccardIndex);

        //Array<double> CM(dataMatrix->expectedBiClusters.size(), foundedBiclusters.size());

        ClassicalHungarian Classis;

        Classis.SetCostMatrix(CM, ClassicalHungarian::MunkresFunc::Max);

        double Value = Classis.RunMunkres() / 8;

        if (res->idMethod < 8)
            singleAvg += Value;

        qDebug() << "IdResult: " << res->idResult << "Method: " << res->idMethod << ", Value: " << Value << " NumberofBic: " << res->foundedBiclusters.size();
    }

}

void Experimental::StartCustom()
{
    //CompareGrandTruth();
    //ARFFPlay();
}

void Experimental::CompareGrandTruth()
{
    std::vector<std::shared_ptr<BiclusteringObject>> AllResults  = engine->db->GetResults(-1, -1, -1, -1);

    bool grandTruth = false;
    std::vector<std::shared_ptr<Bicluster>> expected;

    std::vector<int> cluster1;
    cluster1.push_back(0);
    cluster1.push_back(1);
    cluster1.push_back(2);

    double singleAvg = 0;

    for(std::shared_ptr<BiclusteringObject> res : AllResults)
    {
        if (!grandTruth)
        {
            grandTruth = false;

            QString currentClass = "";
            std::vector<int> currentBic;

            for(Label label : res->dataMatrix->columnLabels)
            {
                if (label.value.size() == 0)
                    continue;

                QStringList tmp = label.value.split('-');

                if (currentClass != tmp[0])
                {
                    currentClass = tmp[0];

                    if (currentBic.size() > 0)
                    {
                        std::shared_ptr<double> ACV;
                        std::shared_ptr<double> Similarity;
                        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, currentBic, ACV, Similarity);
                        expected.push_back(NewBic);
                        currentBic.clear();
                    }

                    currentBic.push_back(label.indexNbr-2);
                }
                else
                {
                    currentBic.push_back(label.indexNbr-2);
                }
            }

            if (currentBic.size() > 0)
            {
                std::shared_ptr<double> ACV;
                std::shared_ptr<double> Similarity;
                std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, currentBic, ACV, Similarity);
                expected.push_back(NewBic);
                currentBic.clear();
            }
        }

        Array<double> CM = res->GetCostMatrixForBiclusters(expected, res->foundedBiclusters, Enums::BiclusterCompareMode::Second, Enums::SimilarityMethods::JaccardIndex);

        //Array<double> CM(dataMatrix->expectedBiClusters.size(), foundedBiclusters.size());

        ClassicalHungarian Classis;

        Classis.SetCostMatrix(CM, ClassicalHungarian::MunkresFunc::Max);

        double Value = Classis.RunMunkres() / 6;

        if (res->idMethod < 8)
            singleAvg += Value;

        qDebug() << "IdResult: " << res->idResult << "Method: " << res->idMethod << ", Value: " << Value;
    }

    qDebug() << "Single Average: " << (singleAvg / 40);
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
            if (single[m]->foundedBiclusters.size() > 0)
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
        }

        //std::shared_ptr<NMF> tmpPtr = std::static_pointer_cast<NMF>(single[best]);

        //tmpPtr->RebuildBiclusters();

        //single[best]->PostProcessingTask();

        test.push_back(single[best]);
    }

    std::vector<MergeType> mt;

    //mt.push_back(Consensus::MergeType::ByACV);
    //mt.push_back(Consensus::MergeType::ByACVHeuristic);
    //mt.push_back(Consensus::MergeType::Standard);

    //for(Consensus::MergeType imt : mt)
    //{
        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(engine->CurrentVmatrix, -1);

        newObject->expectedBiClusterCount = 3;

        newObject->dataMatrix->expectedBiClusterCount = 3;

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

void Experimental::RunAllTriclustering()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(int i = 1; i <= 8; ++i)
    {
        test  = engine->db->GetResults(-1, i, -1, -1);

        int maxBic = 0;

        for(std::shared_ptr<BiclusteringObject> dupa : test)
        {
            if (dupa->foundedBiclusters.size() > maxBic)
                maxBic = dupa->foundedBiclusters.size();
        }

        qDebug() << "Matrix: " << test[0]->dataMatrix->name << " Max: " << maxBic;

        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1);

        //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

        newObject->expectedBiClusterCount = maxBic;

        newObject->dataMatrix->expectedBiClusterCount = maxBic;

        newObject->SetEnsemble(test);

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);
    }
}

void Experimental::RunAllConsensus()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test;

    std::vector<int> ids;

    ids.push_back(1);
    ids.push_back(2);
    ids.push_back(3);
    ids.push_back(4);
    ids.push_back(6);
    ids.push_back(7);
    ids.push_back(8);


    //ids.push_back(5);


    for(uint i = 0; i < ids.size(); ++i)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> tricluster  = engine->db->GetResults(-1, ids[i], Enums::Methods::TRICLUSTERING, -1);

        auto triter = std::remove_if(tricluster.begin(), tricluster.end(), [](std::shared_ptr<BiclusteringObject> r){ return r->foundedBiclusters.size() == 0; });
        tricluster.erase(triter, tricluster.end());

        test  = engine->db->GetResults(-1, ids[i], -1, -1);

        auto riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return r->idMethod == Enums::Methods::CONSENSUS || r->idMethod == Enums::Methods::TRICLUSTERING; });
        test.erase(riter, test.end());

        double average = 0;

        for(std::shared_ptr<BiclusteringObject> dupa : test)
        {
            average += dupa->foundedBiclusters.size();
        }

        int bicCount = (int)(average / test.size());

        qDebug() << "Matrix: " << test[0]->dataMatrix->name << " Average: " << bicCount << " Tricluster: " << tricluster[0]->foundedBiclusters.size();

        std::vector<MergeType> mt;

        //mt.push_back(Consensus::MergeType::ByACV);
        //mt.push_back(Consensus::MergeType::ByACVHeuristic);
        mt.push_back(MergeType::Standard);

        for(MergeType imt : mt)
        {
            std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1);

            //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

            newObject->expectedBiClusterCount = tricluster[0]->foundedBiclusters.size();

            newObject->dataMatrix->expectedBiClusterCount = tricluster[0]->foundedBiclusters.size();

            newObject->SetEnsemble(test);

            newObject->ExtractType = imt;

            std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

            params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

            auto res = newObject->Compute(params);

            if (res != nullptr)
                engine->db->SaveResult(res);
        }

        qDebug() << "Done ;]";
    }
}

void Experimental::RunAllConsensus2()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test;

    std::vector<int> ids;

    //ids.push_back(1);
    ids.push_back(1);
    ids.push_back(3);
    ids.push_back(4);
    ids.push_back(5);
    ids.push_back(6);
    ids.push_back(7);
    ids.push_back(8);

    for(uint i = 0; i < ids.size(); ++i)
    {
        uint start = 5;

        if (i == 0)
            start = 80;

        for(uint s = start; s <= 100; s = s + 5)
        {
            test  = engine->db->GetResults(-1, ids[i], -1, -1);

            auto riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return r->idMethod == Enums::Methods::CONSENSUS || r->idMethod == Enums::Methods::TRICLUSTERING; });
            test.erase(riter, test.end());

            double average = 0;

            for(std::shared_ptr<BiclusteringObject> dupa : test)
            {
                average += dupa->foundedBiclusters.size();
            }

            int bicCount = (int)(average / test.size());

            qDebug() << "Matrix: (id=" << ids[i] << ") " << test[0]->dataMatrix->name << " Average: " << bicCount << " Step bicluster: " << s;

            std::vector<MergeType> mt;

            //mt.push_back(Consensus::MergeType::ByACV);
            //mt.push_back(Consensus::MergeType::ByACVHeuristic);
            mt.push_back(MergeType::Standard);

            for(MergeType imt : mt)
            {
                std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1);

                //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

                newObject->expectedBiClusterCount = s;

                newObject->dataMatrix->expectedBiClusterCount = s;

                newObject->SetEnsemble(test);

                newObject->ExtractType = imt;

                std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

                params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

                auto res = newObject->Compute(params);

                if (res != nullptr)
                    engine->db->SaveResult(res);
            }
        }
        qDebug() << "Done ;]";
    }
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
            if (single[m]->foundedBiclusters.size() > 0)
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
        }

        test.push_back(single[best]);
    }

    std::vector<MergeType> mt;

    //mt.push_back(Consensus::MergeType::ByACV);
    //mt.push_back(Consensus::MergeType::ByACVHeuristic);
    mt.push_back(MergeType::Standard);

    for(MergeType imt : mt)
    {
        //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1);

        std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

        newObject->expectedBiClusterCount = 3;

        newObject->dataMatrix->expectedBiClusterCount = 3;

        newObject->SetEnsemble(test);

        newObject->ExtractType = imt;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);

        //engine->AddBiClusteringTask(newObject);
    }

    //engine->ServeQueue();
}

void Experimental::Squro(QString mode)
{
    QProcess process;

    //process.start("echo \"start\" | nc -q 0 10.0.0.1 3333");

    if (mode == "1")
    {
        std::shared_ptr<Matrix> CurrentVmatrix = engine->db->GetMatrix(9);

        for(int i = 0; i < 3; ++i)
        {
            for(int methodID = 0; methodID < 4; ++methodID)
            {
                std::shared_ptr<BiclusteringObject> newObject;

                switch(methodID)
                {
                    case Enums::PLSA:
                        newObject = std::make_shared<PLSA>(CurrentVmatrix);
                        break;
                    case Enums::LEAST_SQUARE_ERROR:
                        newObject = std::make_shared<LSE>(CurrentVmatrix);
                        break;
                    case Enums::KULLBACK_LIEBER:
                        newObject = std::make_shared<KullbackLeibler>(CurrentVmatrix);
                        break;
                    case Enums::NonSmooth_KULLBACK_LIEBER:
                        newObject = std::make_shared<nsKullbackLeibler>(CurrentVmatrix, 0.5);
                        break;
                }

                engine->AddBiClusteringTask(newObject);
            }

            engine->maxThreadAllowd = 4;
            engine->ServeQueue();
        }

        RunConsensus();
    }
    else if (mode == "2")
    {
        std::shared_ptr<Matrix> CurrentVmatrix = engine->db->GetMatrix(9);

        for(int i = 0; i < 3; ++i)
        {
            for(int methodID = 0; methodID < 4; ++methodID)
            {
                std::shared_ptr<BiclusteringObject> newObject;

                switch(methodID)
                {
                    case Enums::PLSA:
                        newObject = std::make_shared<PLSA>(CurrentVmatrix);
                        break;
                    case Enums::LEAST_SQUARE_ERROR:
                        newObject = std::make_shared<LSE>(CurrentVmatrix);
                        break;
                    case Enums::KULLBACK_LIEBER:
                        newObject = std::make_shared<KullbackLeibler>(CurrentVmatrix);
                        break;
                    case Enums::NonSmooth_KULLBACK_LIEBER:
                        newObject = std::make_shared<nsKullbackLeibler>(CurrentVmatrix, 0.5);
                        break;
                }

                engine->AddBiClusteringTask(newObject);
            }

            engine->maxThreadAllowd = 1;
            engine->ServeQueue();
        }

        RunConsensus();
    }
    else if (mode == "3")
    {
        std::shared_ptr<Matrix> CurrentVmatrix = engine->db->GetMatrix(9);

        for(int i = 0; i < 1; ++i)
        {
            for(int methodID = 0; methodID < 4; ++methodID)
            {
                std::shared_ptr<BiclusteringObject> newObject;

                switch(methodID)
                {
                    case Enums::PLSA:
                        newObject = std::make_shared<PLSA>(CurrentVmatrix);
                        break;
                    case Enums::LEAST_SQUARE_ERROR:
                        newObject = std::make_shared<LSE>(CurrentVmatrix);
                        break;
                    case Enums::KULLBACK_LIEBER:
                        newObject = std::make_shared<KullbackLeibler>(CurrentVmatrix);
                        break;
                    case Enums::NonSmooth_KULLBACK_LIEBER:
                        newObject = std::make_shared<nsKullbackLeibler>(CurrentVmatrix, 0.5);
                        break;
                }

                QByteArray rawData = newObject->Serialize();

                engine->aaclient.connectToHost("localhost");

                engine->aaclient.writeData(rawData);
            }
        }

        //RunConsensus();
    }
    else if (mode == "4")
    {

    }
    else if (mode == "5")
    {

    }
    else if (mode == "6")
    {

    }

    //process.start("echo \"stop\" | nc -q 0 10.0.0.1 3333");

    qDebug() << "Mission Acomplished";
}

void Experimental::LoadKumalBiclusters()
{
    QFile file("/mnt/D/AppData/owncloud/Praca/Publikacje/2016/Elsevier/real_data_results/gds_results.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);

        QString matrix;
        QString method;
        int bicNum = 0;
        QList<QString> inMatrix;

        std::shared_ptr<BiclusteringObject> bicObject;
        std::shared_ptr<Matrix> dataMatrix;

        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (line.left(3) == "GDS")
            {
                QStringList tmp = line.split(' ');

                if (!inMatrix.contains(tmp[0])){
                    inMatrix.append(tmp[0]);

                    dataMatrix = std::make_shared<Matrix>("/mnt/D/AppData/owncloud/Praca/Publikacje/2016/Elsevier/real_data_results/" + tmp[0] + ".soft");
                    dataMatrix->name = tmp[0];
                    dataMatrix->group = "Kemal Eren";
                }

                if (tmp[0] != matrix || tmp[1] != method){

                    if (bicNum > 0){
                        qDebug() << line << " biclust number: " << bicNum;

                        //Save biclusters
                        engine->db->SaveResult(bicObject);

                        bicNum = 0;
                    }

                    matrix = tmp[0];
                    method = tmp[1];

                    Enums::Methods methodID;


                    if (method == "Spectral")
                        methodID = Enums::Methods::Spectral;
                    else if (method == "Plaid")
                        methodID = Enums::Methods::Plaid;
                    else if (method == "OPSM")
                        methodID = Enums::Methods::OPSM;
                    else if (method == "BBC")
                        methodID = Enums::Methods::BBC;
                    else if (method == "CPB")
                        methodID = Enums::Methods::CPB;
                    else if (method == "FABIA")
                        methodID = Enums::Methods::FABIA;
                    else if (method == "QUBIC")
                        methodID = Enums::Methods::QUBIC;
                    else if (method == "ISA")
                        methodID = Enums::Methods::ISA;
                    else if (method == "ChengandChurch")
                        methodID = Enums::Methods::ChengandChurch;
                    else if (method == "BiMax")
                        methodID = Enums::Methods::BiMax;
                    else if (method == "xMOTIFs")
                        methodID = Enums::Methods::xMOTIFs;


                    bicObject = std::make_shared<BiclusteringObject>(dataMatrix, methodID, -1, -1);
                }

                QString rows = in.readLine().trimmed();
                QString columns = in.readLine().trimmed();

                std::vector<int> cluster1;
                std::vector<int> cluster2;

                tmp = rows.split(' ');

                for(int i = 1; i < tmp.size(); ++i)
                {
                    cluster1.push_back(tmp[i].toInt());
                }

                tmp = columns.split(' ');

                for(int i = 1; i < tmp.size(); ++i)
                {
                    cluster2.push_back(tmp[i].toInt());
                }

                if (cluster1.size() > 0 && cluster2.size() > 0)
                {
                    bicNum++;

                    std::shared_ptr<Bicluster> fb = std::make_shared<Bicluster>(-1, cluster1, cluster2, nullptr, nullptr);

                    bicObject->foundedBiclusters.push_back(fb);
                }
            }
        }
    }
}
