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

                for(int c = 0; c < classes.size(); ++c)
                {
                    if (std::get<0>(classes[c]) == label.value)
                    {
                        currClass = std::get<1>(classes[c]);
                        break;
                    }
                }

                bool added = false;

                for(int b = 0; b < dicLabel.size(); ++b)
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
