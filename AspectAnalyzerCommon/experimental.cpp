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
    std::shared_ptr<Matrix> dupa = std::make_shared<Matrix>("/mnt/E/TCGA/DataMatrixTCGA5_mutect.vmatrix");

    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(1, 1, -1, -1);

    for(uint i = 0; i < dupa->data.n_rows; ++i)
    {
        if (i < 505)
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Prostate");
        else if (i > 1008)
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Head and Neck");
        else
            dupa->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Thyroid");
    }

    test[0]->dataMatrix = dupa;

    //test[0]->GenerateARFFFile("/mnt/E/TCGA/test.arff", 1);

    std::vector<int> indexes;

    for(std::shared_ptr<Bicluster> bic : test[0]->foundedBiclusters)
    {
        for(int b : bic->cluster2)
        {
            auto iter = std::find_if(indexes.begin(), indexes.end(), [b](int s){ return s == b; });

            if (iter == indexes.end())
                indexes.push_back(b);
        }
    }

    test[0]->GenerateARFFFile("/mnt/E/TCGA/test_50.arff", 1, indexes);

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

    test[0]->GenerateARFFFile("/mnt/E/TCGA/test_rafal.arff", 1, indexes2);

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

    test[0]->GenerateARFFFile("/mnt/E/TCGA/test_super.arff", 1, indexes3);

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

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);

        //engine->AddBiClusteringTask(newObject);
    }

    //engine->ServeQueue();
}
