#include "experimental.h"

void Experimental::ExportNMFs()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, -1, -1, -1);

    for(std::shared_ptr<BiclusteringObject> single : test)
    {
        std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(single);

        if (tmpPtr != nullptr)
        {
            tmpPtr->HMatrix.save((QString::number(tmpPtr->idResult) + "_H.mat").toStdString(), arma::arma_ascii);
            tmpPtr->WMatrix.save((QString::number(tmpPtr->idResult) + "_W.mat").toStdString(), arma::arma_ascii);
        }
    }
}

void Experimental::TestTriclustering()
{
    std::vector<std::shared_ptr<BiclusteringObject>> test = engine->db->GetResults(-1, 1, Enums::Methods::PLSA, -1);

    std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1, -1, "");

    newObject->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

    newObject->SetEnsemble(test);

    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

    params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

    newObject->Compute(params);
}

void Experimental::ImportMatlabResults()
{
    QFile filestream("D:\\AppData\\owncloud\\Praca\\PolSl\\AspectAnalyzer\\importALL.txt");

    if(!filestream.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&filestream);

        QMap<QString, std::shared_ptr<Matrix>> map;

        while(!in.atEnd()) {

            QString currentMatrix = in.readLine().trimmed();

            if (!map.contains(currentMatrix))
            {
                std::shared_ptr<Matrix> newItem = std::make_shared<Matrix>("D:\\AppData\\share\\data\\" + currentMatrix);

                newItem->idMatrix = std::make_shared<int>(engine->db->SaveMatrix(newItem->data, currentMatrix, "New Batch", Enums::MatrixType::V, -1));

                engine->db->SaveBiclusters(newItem->expectedBiClusters, *newItem->idMatrix, -1);
                engine->db->SaveLabels(newItem->rowLabels, *newItem->idMatrix);
                engine->db->SaveLabels(newItem->columnLabels, *newItem->idMatrix);

                map[currentMatrix] = newItem;

                qDebug() << map.size();
            }

            Enums::Methods method;

            QString lineMethod = in.readLine().trimmed();

            if (lineMethod == "BiMax")
                method = Enums::Methods::BiMax;
            else if (lineMethod == "CC")
                method = Enums::Methods::ChengandChurch;
            else if (lineMethod == "Floc")
                method = Enums::Methods::Floc;
            else if (lineMethod == "ITL")
                method = Enums::Methods::ITL;
            else if (lineMethod == "FABIA")
                method = Enums::Methods::FABIA;
            else if (lineMethod == "kSpectral")
                method = Enums::Methods::kSpectral;
            else if (lineMethod == "Plaid")
                method = Enums::Methods::Plaid;
            else if (lineMethod == "QUBIC")
                method = Enums::Methods::QUBIC;
            else if (lineMethod == "XMotifs")
                method = Enums::Methods::xMOTIFs;

            int numOfBiClusters = in.readLine().trimmed().toInt();

            std::shared_ptr<BiclusteringObject> currRes = std::make_shared<BiclusteringObject>(map[currentMatrix], method, -1, 0, "NewBatch Import");

            for(int i = 0; i < numOfBiClusters; ++i)
            {
                QString cluster1Line = in.readLine().trimmed();
                QStringList cluster1Items = cluster1Line.split(' ');
                std::vector<int> cluster1;

                for(QString item : cluster1Items)
                {
                    cluster1.push_back(item.toInt() - 1);
                }

                QString cluster2Line = in.readLine().trimmed();
                QStringList cluster2Items = cluster2Line.split(' ');
                std::vector<int> cluster2;

                for(QString item : cluster2Items)
                {
                    cluster2.push_back(item.toInt() - 1);
                }

                std::shared_ptr<Bicluster> newBic = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                currRes->foundedBiclusters.push_back(newBic);
            }

            engine->db->SaveResult(currRes);
        }
    }
}

void Experimental::RunAllEnsemble(int idMatrix, QString desc)
{
    std::vector<std::shared_ptr<BiclusteringObject>> test  = engine->db->GetResults(-1, idMatrix, -1, -1);

    std::vector<MergeType> mt;

    mt.push_back(MergeType::ByACV);
    //mt.push_back(MergeType::ByACVHeuristic);
    //mt.push_back(MergeType::Standard);
    //mt.push_back(MergeType::None);
    //mt.push_back(MergeType::GainFunction);

    std::vector<QString> descs;

    descs.push_back("ByACV");
    //descs.push_back("ByACVHeuristic");
    //descs.push_back("Standard");
    //descs.push_back("None");
    //descs.push_back("GainFunction");

    std::vector<Enums::FeatureType> ft;

    ft.push_back(Enums::FeatureType::ACV);
    //ft.push_back(Enums::FeatureType::ASR);
    //ft.push_back(Enums::FeatureType::MSR);
    //ft.push_back(Enums::FeatureType::SMSR);
    //ft.push_back(Enums::FeatureType::Variance);


    std::vector<QString> descsFeat;

    descsFeat.push_back("ACV");
    descsFeat.push_back("ASR");
    descsFeat.push_back("MSR");
    descsFeat.push_back("SMSR");
    descsFeat.push_back("Variance");

    int f = 0;

    for(Enums::FeatureType ift : ft)
    {
        int d = 0;

        for(MergeType imt : mt)
        {
            std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1, 0, desc + "_" + descs[d++] + "_" + descsFeat[f]);

            //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

            newObject->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

            newObject->dataMatrix->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

            newObject->SetEnsemble(test);

            newObject->ExtractType = imt;

            newObject->qualityMeasure = ift;

            std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

            params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

            //auto res = newObject->Compute(params);

            //engine->db->SaveResult(res);

            if (imt == MergeType::GainFunction)
            {

                    newObject->penaltyForGainFunction = f + 1;
                    newObject->desc += "_" + QString::number(f + 1);

            }


            engine->AddBiClusteringTask(newObject);
        }

        f++;
    }

    std::shared_ptr<TriClustering> newObjectT = std::make_shared<TriClustering>(test[0]->dataMatrix, -1, 0, desc);

    newObjectT->expectedBiClusterCount = newObjectT->dataMatrix->expectedBiClusterCount;

    newObjectT->dataMatrix->expectedBiClusterCount = newObjectT->dataMatrix->expectedBiClusterCount;

    newObjectT->SetEnsemble(test);

    //newObject->ExtractType = imt;

    //std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> paramsT;

    //paramsT.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObjectT->dataMatrix->expectedBiClusterCount));

    //auto res = newObject->Compute(params);

    //engine->db->SaveResult(res);

    engine->AddBiClusteringTask(newObjectT);

    qDebug() << idMatrix;
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
                std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, single);
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

void Experimental::ImportKumalResults(int repNum)
{
    for(int m = 0; m < repNum; ++m)
    {
        qDebug() << "Model rep " << m;

        QString path = "D:\\AppData\\owncloud\\Praca\\PolSl\\AspectAnalyzer\\Dane\\model\\rep_" + QString::number(m);

        std::vector<std::shared_ptr<BiclusteringObject>> importRes;

        std::shared_ptr<Matrix> currMatrix;

        QDirIterator it(path, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString file = it.next();
            QStringList temp = file.split('_');

            //for(auto part : temp)
            //{
                //qDebug() << part;
            //}

            //qDebug() << "File: " << file << " - " << temp.size() << "\n";

            if (temp.size() == 6)
            {
                if (temp[5] != "found.biclusters")
                    continue;

                //qDebug() << "Model: " << temp[2] << ". Algorithm: " << temp[4] << ". Extension: " << temp[5] << "\n";

                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    Enums::Methods Method = Enums::Methods::CONSENSUS;

                    if (temp[4] == "BBC")
                            Method = Enums::Methods::BBC;
                    else if (temp[4] == "BiMax")
                        Method = Enums::Methods::BiMax;
                    else if (temp[4] == "Cheng and Church")
                        Method = Enums::Methods::ChengandChurch;
                    else if (temp[4] == "COALESCE")
                        Method = Enums::Methods::COALESCE;
                    else if (temp[4] == "CPB")
                        Method = Enums::Methods::CPB;
                    else if (temp[4] == "FABIA")
                        Method = Enums::Methods::FABIA;
                    else if (temp[4] == "ISA")
                        Method = Enums::Methods::ISA;
                    else if (temp[4] == "OPSM")
                        Method = Enums::Methods::OPSM;
                    else if (temp[4] == "Plaid")
                        Method = Enums::Methods::Plaid;
                    else if (temp[4] == "QUBIC")
                        Method = Enums::Methods::QUBIC;
                    else if (temp[4] == "Spectral")
                        Method = Enums::Methods::Spectral;
                    else if (temp[4] == "xMOTIFs")
                        Method = Enums::Methods::xMOTIFs;


                    std::shared_ptr<BiclusteringObject> currBic = std::make_shared<BiclusteringObject>(currMatrix, Method, -1, 0, "Kumal Import");

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currBic->foundedBiclusters.push_back(newItem);
                        }
                    }

                    importRes.push_back(currBic);
                }
            }
            else if (temp.size() == 5)
            {
                //qDebug() << "Expected biclusters \n";
                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currMatrix->expectedBiClusters.push_back(newItem);
                        }
                    }

                    currMatrix->expectedBiClusterCount = currMatrix->expectedBiClusters.size();

                    currMatrix->idMatrix = std::make_shared<int>(engine->db->SaveMatrix(currMatrix->data, currMatrix->name, currMatrix->group, Enums::MatrixType::V, -1));
                    engine->db->SaveBiclusters(currMatrix->expectedBiClusters, *currMatrix->idMatrix, -1);
                    engine->db->SaveLabels(currMatrix->rowLabels, *currMatrix->idMatrix);
                    engine->db->SaveLabels(currMatrix->columnLabels, *currMatrix->idMatrix);
                }
            }
            else if (temp.size() > 2)
            {
                if (currMatrix != nullptr)
                {
                    for(auto bic : importRes)
                    {
                        std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                        for(auto fb : bic->foundedBiclusters)
                        {
                            newBic->foundedBiclusters.push_back(fb);
                        }
                        newBic->idMatrix = *currMatrix->idMatrix;
                        engine->db->SaveResult(newBic);
                    }
                }

                importRes.clear();

                //qDebug() << "Dataset model: " << temp[2] << "\n";

                currMatrix = std::make_shared<Matrix>(file);
                currMatrix->name = temp[2];
                currMatrix->group = "rep" + QString::number(m);
            }
        }

        if (currMatrix != nullptr)
        {
            for(auto bic : importRes)
            {
                std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                for(auto fb : bic->foundedBiclusters)
                {
                    newBic->foundedBiclusters.push_back(fb);
                }
                newBic->idMatrix = *currMatrix->idMatrix;
                engine->db->SaveResult(newBic);
            }
        }
    }

    qDebug() << "Done";
}

void Experimental::ImportKumalResultsNoise(int repNum)
{
    for(int m = 0; m < repNum; ++m)
    {
        qDebug() << "Noise rep " << m;

        QString path = "D:\\AppData\\owncloud\\Praca\\PolSl\\AspectAnalyzer\\Dane\\noise\\rep_" + QString::number(m);

        std::vector<std::shared_ptr<BiclusteringObject>> importRes;

        std::shared_ptr<Matrix> currMatrix;

        QDirIterator it(path, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString file = it.next();
            QStringList temp = file.split('_');

//            int i = 0;
//            for(auto part : temp)
//            {
//                qDebug() << i++ << ": " << part;
//            }

//            continue;

            //qDebug() << "File: " << file << " - " << temp.size() << "\n";

            if (temp.size() == 7)
            {
                if (temp[6] != "found.biclusters")
                    continue;

                //qDebug() << "Model: " << temp[2] << ". Algorithm: " << temp[5] << ". Extension: " << temp[6] << "\n";

                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    Enums::Methods Method = Enums::Methods::CONSENSUS;

                    if (temp[5] == "BBC")
                            Method = Enums::Methods::BBC;
                    else if (temp[5] == "BiMax")
                        Method = Enums::Methods::BiMax;
                    else if (temp[5] == "Cheng and Church")
                        Method = Enums::Methods::ChengandChurch;
                    else if (temp[5] == "COALESCE")
                        Method = Enums::Methods::COALESCE;
                    else if (temp[5] == "CPB")
                        Method = Enums::Methods::CPB;
                    else if (temp[5] == "FABIA")
                        Method = Enums::Methods::FABIA;
                    else if (temp[5] == "ISA")
                        Method = Enums::Methods::ISA;
                    else if (temp[5] == "OPSM")
                        Method = Enums::Methods::OPSM;
                    else if (temp[5] == "Plaid")
                        Method = Enums::Methods::Plaid;
                    else if (temp[5] == "QUBIC")
                        Method = Enums::Methods::QUBIC;
                    else if (temp[5] == "Spectral")
                        Method = Enums::Methods::Spectral;
                    else if (temp[5] == "xMOTIFs")
                        Method = Enums::Methods::xMOTIFs;


                    std::shared_ptr<BiclusteringObject> currBic = std::make_shared<BiclusteringObject>(currMatrix, Method, -1, 0, "Kumal Import");

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currBic->foundedBiclusters.push_back(newItem);
                        }
                    }

                    importRes.push_back(currBic);
                }
            }
            else if (temp.size() == 6)
            {
                //qDebug() << "Expected biclusters \n";
                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currMatrix->expectedBiClusters.push_back(newItem);
                        }
                    }

                    currMatrix->expectedBiClusterCount = currMatrix->expectedBiClusters.size();

                    currMatrix->idMatrix = std::make_shared<int>(engine->db->SaveMatrix(currMatrix->data, currMatrix->name, currMatrix->group, Enums::MatrixType::V, -1));
                    engine->db->SaveBiclusters(currMatrix->expectedBiClusters, *currMatrix->idMatrix, -1);
                    engine->db->SaveLabels(currMatrix->rowLabels, *currMatrix->idMatrix);
                    engine->db->SaveLabels(currMatrix->columnLabels, *currMatrix->idMatrix);
                }
            }
            else if (temp.size() > 2)
            {
                if (currMatrix != nullptr)
                {
                    for(auto bic : importRes)
                    {
                        std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                        for(auto fb : bic->foundedBiclusters)
                        {
                            newBic->foundedBiclusters.push_back(fb);
                        }
                        newBic->idMatrix = *currMatrix->idMatrix;
                        engine->db->SaveResult(newBic);
                    }
                }

                importRes.clear();

                //qDebug() << "Dataset model: " << temp[2] << "\n";

                currMatrix = std::make_shared<Matrix>(file);
                temp[4].resize(temp[4].size() - 8);
                currMatrix->name = temp[2] + "_" + temp[4];
                currMatrix->group = "rep" + QString::number(m);
            }
        }

        if (currMatrix != nullptr)
        {
            for(auto bic : importRes)
            {
                std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                for(auto fb : bic->foundedBiclusters)
                {
                    newBic->foundedBiclusters.push_back(fb);
                }
                newBic->idMatrix = *currMatrix->idMatrix;
                engine->db->SaveResult(newBic);
            }
        }
    }

    qDebug() << "Done";
}

void Experimental::ImportKumalResultsNumber(int repNum)
{
    for(int m = 0; m < repNum; ++m)
    {
        qDebug() << "Number rep " << m;

        QString path = "D:\\AppData\\owncloud\\Praca\\PolSl\\AspectAnalyzer\\Dane\\number\\rep_" + QString::number(m);

        std::vector<std::shared_ptr<BiclusteringObject>> importRes;

        std::shared_ptr<Matrix> currMatrix;

        QDirIterator it(path, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString file = it.next();
            QStringList temp = file.split('_');

//            int i = 0;
//            for(auto part : temp)
//            {
//                qDebug() << i++ << ": " << part;
//            }
//            continue;

            //qDebug() << "File: " << file << " - " << temp.size() << "\n";

            if (temp.size() == 8)
            {
                if (temp[7] != "found.biclusters")
                    continue;

                //qDebug() << "Model: " << temp[2] << ". Algorithm: " << temp[6] << ". Extension: " << temp[7] << "\n";

                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    Enums::Methods Method = Enums::Methods::CONSENSUS;

                    if (temp[6] == "BBC")
                            Method = Enums::Methods::BBC;
                    else if (temp[6] == "BiMax")
                        Method = Enums::Methods::BiMax;
                    else if (temp[6] == "Cheng and Church")
                        Method = Enums::Methods::ChengandChurch;
                    else if (temp[6] == "COALESCE")
                        Method = Enums::Methods::COALESCE;
                    else if (temp[6] == "CPB")
                        Method = Enums::Methods::CPB;
                    else if (temp[6] == "FABIA")
                        Method = Enums::Methods::FABIA;
                    else if (temp[6] == "ISA")
                        Method = Enums::Methods::ISA;
                    else if (temp[6] == "OPSM")
                        Method = Enums::Methods::OPSM;
                    else if (temp[6] == "Plaid")
                        Method = Enums::Methods::Plaid;
                    else if (temp[6] == "QUBIC")
                        Method = Enums::Methods::QUBIC;
                    else if (temp[6] == "Spectral")
                        Method = Enums::Methods::Spectral;
                    else if (temp[6] == "xMOTIFs")
                        Method = Enums::Methods::xMOTIFs;


                    std::shared_ptr<BiclusteringObject> currBic = std::make_shared<BiclusteringObject>(currMatrix, Method, -1, 0, "Kumal Import");

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currBic->foundedBiclusters.push_back(newItem);
                        }
                    }

                    importRes.push_back(currBic);
                }
            }
            else if (temp.size() == 7)
            {
                //qDebug() << "Expected biclusters \n";
                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currMatrix->expectedBiClusters.push_back(newItem);
                        }
                    }

                    currMatrix->expectedBiClusterCount = currMatrix->expectedBiClusters.size();

                    currMatrix->idMatrix = std::make_shared<int>(engine->db->SaveMatrix(currMatrix->data, currMatrix->name, currMatrix->group, Enums::MatrixType::V, -1));
                    engine->db->SaveBiclusters(currMatrix->expectedBiClusters, *currMatrix->idMatrix, -1);
                    engine->db->SaveLabels(currMatrix->rowLabels, *currMatrix->idMatrix);
                    engine->db->SaveLabels(currMatrix->columnLabels, *currMatrix->idMatrix);
                }
            }
            else if (temp.size() > 2)
            {
                if (currMatrix != nullptr)
                {
                    for(auto bic : importRes)
                    {
                        std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                        for(auto fb : bic->foundedBiclusters)
                        {
                            newBic->foundedBiclusters.push_back(fb);
                        }
                        newBic->idMatrix = *currMatrix->idMatrix;
                        engine->db->SaveResult(newBic);
                    }
                }

                importRes.clear();

                //qDebug() << "Dataset model: " << temp[2] << "\n";

                currMatrix = std::make_shared<Matrix>(file);
                temp[5].resize(temp[5].size() - 8);
                currMatrix->name = temp[2] + "_" + temp[4] + "_" + temp[5];
                currMatrix->group = "rep" + QString::number(m);
            }
        }

        if (currMatrix != nullptr)
        {
            for(auto bic : importRes)
            {
                std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                for(auto fb : bic->foundedBiclusters)
                {
                    newBic->foundedBiclusters.push_back(fb);
                }
                newBic->idMatrix = *currMatrix->idMatrix;
                engine->db->SaveResult(newBic);
            }
        }
    }

    qDebug() << "Done";
}

void Experimental::ImportKumalResultsOverlap(int repNum)
{
    for(int m = 0; m < repNum; ++m)
    {
        qDebug() << "Overlap rep " << m;

        QString path = "D:\\AppData\\owncloud\\Praca\\PolSl\\AspectAnalyzer\\Dane\\overlap\\rep_" + QString::number(m);

        std::vector<std::shared_ptr<BiclusteringObject>> importRes;

        std::shared_ptr<Matrix> currMatrix;

        QDirIterator it(path, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString file = it.next();
            QStringList temp = file.split('_');

//            int i = 0;
//            for(auto part : temp)
//            {
//                qDebug() << i++ << ": " << part;
//            }
//            continue;

            //qDebug() << "File: " << file << " - " << temp.size() << "\n";

            if (temp.size() == 11)
            {
                if (temp[10] != "found.biclusters")
                    continue;

                //qDebug() << "Model: " << temp[2] << ". Algorithm: " << temp[9] << ". Extension: " << temp[10] << "\n";

                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    Enums::Methods Method = Enums::Methods::CONSENSUS;

                    if (temp[9] == "BBC")
                            Method = Enums::Methods::BBC;
                    else if (temp[9] == "BiMax")
                        Method = Enums::Methods::BiMax;
                    else if (temp[9] == "Cheng and Church")
                        Method = Enums::Methods::ChengandChurch;
                    else if (temp[9] == "COALESCE")
                        Method = Enums::Methods::COALESCE;
                    else if (temp[9] == "CPB")
                        Method = Enums::Methods::CPB;
                    else if (temp[9] == "FABIA")
                        Method = Enums::Methods::FABIA;
                    else if (temp[9] == "ISA")
                        Method = Enums::Methods::ISA;
                    else if (temp[9] == "OPSM")
                        Method = Enums::Methods::OPSM;
                    else if (temp[9] == "Plaid")
                        Method = Enums::Methods::Plaid;
                    else if (temp[9] == "QUBIC")
                        Method = Enums::Methods::QUBIC;
                    else if (temp[9] == "Spectral")
                        Method = Enums::Methods::Spectral;
                    else if (temp[9] == "xMOTIFs")
                        Method = Enums::Methods::xMOTIFs;


                    std::shared_ptr<BiclusteringObject> currBic = std::make_shared<BiclusteringObject>(currMatrix, Method, -1, 0, "Kumal Import");

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currBic->foundedBiclusters.push_back(newItem);
                        }
                    }

                    importRes.push_back(currBic);
                }
            }
            else if (temp.size() == 10)
            {
                //qDebug() << "Expected biclusters \n";
                QFile filestream(file);

                if(!filestream.open(QIODevice::ReadOnly))
                {
                    //QMessageBox::information(0, "error", file.errorString());
                }
                else
                {
                    QTextStream in(&filestream);

                    while(!in.atEnd()) {

                        QString line = in.readLine().trimmed();

                        if (line.size() > 0)
                        {
                            QStringList bictmp = line.split(' ');

                            std::vector<int> cluster1;

                            for(QString val : bictmp)
                            {
                                cluster1.push_back(val.toInt());
                            }

                            line = in.readLine().trimmed();
                            bictmp = line.split(' ');

                            std::vector<int> cluster2;

                            for(QString val : bictmp)
                            {
                                cluster2.push_back(val.toInt());
                            }

                            std::shared_ptr<Bicluster> newItem = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                            currMatrix->expectedBiClusters.push_back(newItem);
                        }
                    }

                    currMatrix->expectedBiClusterCount = currMatrix->expectedBiClusters.size();

                    currMatrix->idMatrix = std::make_shared<int>(engine->db->SaveMatrix(currMatrix->data, currMatrix->name, currMatrix->group, Enums::MatrixType::V, -1));
                    engine->db->SaveBiclusters(currMatrix->expectedBiClusters, *currMatrix->idMatrix, -1);
                    engine->db->SaveLabels(currMatrix->rowLabels, *currMatrix->idMatrix);
                    engine->db->SaveLabels(currMatrix->columnLabels, *currMatrix->idMatrix);
                }
            }
            else if (temp.size() > 2)
            {
                if (currMatrix != nullptr)
                {
                    for(auto bic : importRes)
                    {
                        std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                        for(auto fb : bic->foundedBiclusters)
                        {
                            newBic->foundedBiclusters.push_back(fb);
                        }
                        newBic->idMatrix = *currMatrix->idMatrix;
                        engine->db->SaveResult(newBic);
                    }
                }

                importRes.clear();

                //qDebug() << "Dataset model: " << temp[2] << "\n";
                temp[8].resize(temp[8].size() - 8);
                currMatrix = std::make_shared<Matrix>(file);
                currMatrix->name = temp[2] + "_" + temp[4] + "_" + temp[5] + "_" + temp[6] + "_" + temp[7] + "_" + temp[8];
                currMatrix->group = "rep" + QString::number(m);
            }
        }

        if (currMatrix != nullptr)
        {
            for(auto bic : importRes)
            {
                std::shared_ptr<BiclusteringObject> newBic = std::make_shared<BiclusteringObject>(currMatrix, (Enums::Methods)bic->idMethod, -1, 0, "Kumal Import");
                for(auto fb : bic->foundedBiclusters)
                {
                    newBic->foundedBiclusters.push_back(fb);
                }
                newBic->idMatrix = *currMatrix->idMatrix;
                engine->db->SaveResult(newBic);
            }
        }
    }

    qDebug() << "Done";
}

void Experimental::PringInfo(int taskID, QString filename)
{
    std::vector<std::shared_ptr<BiclusteringObject>> TmpTask = engine->db->GetResults(taskID,-1,-1,-1);

    std::shared_ptr<BiclusteringObject> task = TmpTask[0];

    QFile retVal(filename);

    retVal.open(QIODevice::Append | QIODevice::Text);

    QTextStream out(&retVal);

    if (task->foundedBiclusters.size() > 0)
    {
        task->PostProcessingTask();

        double ASR = 0;
        double MSR = 0;
        double ACV = 0;
        double Similarity = 0;

        if (task->foundedBiclusters.size() > 0)
        {
            for(uint i = 0; i < task->foundedBiclusters.size(); ++i)
            {
                if (task->foundedBiclusters[i]->cluster1.size() > 0 && task->foundedBiclusters[i]->cluster2.size() > 0)
                {
                    //ASR += task->dataMatrix->AverageSpearmansRank(task->foundedBiclusters[i]->cluster1, task->foundedBiclusters[i]->cluster2);
                    //MSR += task->dataMatrix->MeanSquaredResidue(task->foundedBiclusters[i]->cluster1, task->foundedBiclusters[i]->cluster2);
                    //ACV += task->dataMatrix->AverageCorrelationValue(task->foundedBiclusters[i]->cluster1, task->foundedBiclusters[i]->cluster2);
                }
            }

            ASR /= task->foundedBiclusters.size();
            MSR /= task->foundedBiclusters.size();
        }


        //qDebug() << task->idResult << ";" << *task->dataMatrix->idMatrix << ";" << task->dataMatrix->name << ";" << task->idMethod << ";" << ACV << ";" << Similarity  << ";" << task->Recovery()  << ";" << task->Relevance() << ";" << ASR << ";" << MSR << ";" << task->desc;
        //out << task->idResult << ";" << *task->dataMatrix->idMatrix << ";" << task->dataMatrix->name << ";" << task->idMethod << ";" << ACV << ";" << Similarity  << ";" << task->Recovery()  << ";" << task->Relevance() << ";" << ASR << ";" << MSR << ";" << task->desc << "\n";
    }
    else
    {
        out << task->idResult << ";" << *task->dataMatrix->idMatrix << ";" << task->dataMatrix->name << ";" << task->idMethod << ";0;0;0;0;0;0;" << task->desc << "\n";
    }
}

void Experimental::Custom3SoftExp()
{
    //std::shared_ptr<Matrix> m = std::make_shared<Matrix>(QString("D:\\tmp\\2015_item.vmatrix"));
    //engine->db->SaveMatrix(m->data, "item", "2015", Enums::V, -1);
    //engine->db->SaveLabels(m->rowLabels, 1);
    //engine->db->SaveLabels(m->columnLabels, 1);

//    m = std::make_shared<Matrix>("E:\\pfoszner\\2015_money.vmatrix");
//    engine->db->SaveMatrix(m->data, "money", "2015", Enums::V, -1);

    //m = std::make_shared<Matrix>(QString("D:\\tmp\\2016_item.vmatrix"));
    //engine->db->SaveMatrix(m->data, "item", "2016", Enums::V, -1);
    //engine->db->SaveLabels(m->rowLabels, 2);
    //engine->db->SaveLabels(m->columnLabels, 2);

//    m = std::make_shared<Matrix>("E:\\pfoszner\\2016_money.vmatrix");
//    engine->db->SaveMatrix(m->data, "money", "2016", Enums::V, -1);

    //m = std::make_shared<Matrix>(QString("D:\\tmp\\2017_item.vmatrix"));
    //engine->db->SaveMatrix(m->data, "item", "2017", Enums::V, -1);
    //engine->db->SaveLabels(m->rowLabels, 3);
    //engine->db->SaveLabels(m->columnLabels, 3);

//    m = std::make_shared<Matrix>("E:\\pfoszner\\2017_money.vmatrix");
//    engine->db->SaveMatrix(m->data, "money", "2017", Enums::V, -1);

//    for(int m = 1; m < 4; ++m)
//        RunNMF(m, 2, 8, 1, 1);

    for(int m = 1; m < 4; ++m)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> results = engine->db->GetResults(-1, m, -1, -1);

        for(std::shared_ptr<BiclusteringObject> r : results)
        {
            std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(r);

            if (tmpPtr != nullptr)
            {
                tmpPtr->expectedBiClusterCount = tmpPtr->foundedBiclusters.size();
                tmpPtr->dataMatrix->expectedBiClusterCount = tmpPtr->foundedBiclusters.size();
                tmpPtr->RebuildBiclusters();
                tmpPtr->saveToLocalFile = "D:\\tmp\\results\\" + QString::number(tmpPtr->idResult);
                tmpPtr->SaveNMFToLocalFile();
            }
        }
    }

}

void Experimental::StartCustom(QString mode)
{
    //CheckNMFBiclusNumberSearch();

    Custom3SoftExp();

    //ImportKumalResults(20);
    //ImportKumalResultsNoise(20);
    //ImportKumalResultsNumber(20);
    //ImportKumalResultsOverlap(20);

//return;

    //ImportMatlabResults();

    //ExportNMFs();

    //RunAllEnsemble(1, "NewBatch");

    return;

    for(int i = 14037; i <= 15164; ++i)
    {
        //if (i > 14248)
            //PringInfo(i, "Final.csv");
    }

    QStringList params = mode.split(';');

    int start = params[0].toInt();
    int stop = params[1].toInt();

    //return;
    std::vector<int> mats = engine->db->getGroupOfMatrices("rep0");

    int count = 0;

    for(int mat : mats)
    {
        ++count;



/*
        RunSingleNMF(mat, Enums::Methods::PLSA);
        RunSingleNMF(mat, Enums::Methods::LEAST_SQUARE_ERROR);
        RunSingleNMF(mat, Enums::Methods::KULLBACK_LIEBER);
        RunSingleNMF(mat, Enums::Methods::NonSmooth_KULLBACK_LIEBER);
        qDebug() << count << " of " << mats.size() << " witch is " << (round((count * 1000.0) / mats.size()) / 10) << " %";



        continue;

*/
        if (count <= start || count > stop)
            continue;


        std::vector<Enums::Methods> methods;

        methods.push_back(Enums::Methods::PLSA);
        methods.push_back(Enums::Methods::LEAST_SQUARE_ERROR);
        methods.push_back(Enums::Methods::KULLBACK_LIEBER);
        methods.push_back(Enums::Methods::NonSmooth_KULLBACK_LIEBER);
        methods.push_back(Enums::Methods::FABIA);
        methods.push_back(Enums::Methods::QUBIC);
        methods.push_back(Enums::Methods::Spectral);
        methods.push_back(Enums::Methods::Plaid);
        methods.push_back(Enums::Methods::OPSM);
        methods.push_back(Enums::Methods::BBC);
        methods.push_back(Enums::Methods::CPB);
        methods.push_back(Enums::Methods::ISA);
        methods.push_back(Enums::Methods::ChengandChurch);
        methods.push_back(Enums::Methods::BiMax);
        methods.push_back(Enums::Methods::xMOTIFs);
        methods.push_back(Enums::Methods::COALESCE);

        RunTriclustering(mat, methods, "FinalMeasureTest");
        //RunConsensus(mat, methods, "FinalMeasureTest");

        qDebug() << count << " of " << mats.size() << " witch is " << (round((count * 1000.0) / mats.size()) / 10) << " %";
    }

    engine->ServeQueue();

    /*
    PringInfo(10313, "dupa.csv");
    PringInfo(10314, "dupa.csv");
    PringInfo(10316, "dupa.csv");
    PringInfo(10317, "dupa.csv");
    PringInfo(10318, "dupa.csv");
    PringInfo(10311, "dupa.csv");
    PringInfo(10312, "dupa.csv");
    PringInfo(10315, "dupa.csv");
    PringInfo(10319, "dupa.csv");
    PringInfo(10320, "dupa.csv");
    PringInfo(10321, "dupa.csv");
    PringInfo(10322, "dupa.csv");
    PringInfo(10323, "dupa.csv");
    PringInfo(10324, "dupa.csv");
    PringInfo(10325, "dupa.csv");
    PringInfo(10326, "dupa.csv");
    PringInfo(10327, "dupa.csv");
    PringInfo(10328, "dupa.csv");
    PringInfo(10330, "dupa.csv");
    PringInfo(10332, "dupa.csv");
*/

    //RunConsensus(1);


    //return;
    //PringInfo(14033, "dupa.csv");
    //PringInfo(14034, "dupa.csv");
    //PringInfo(14035, "dupa.csv");
    //PringInfo(14036, "dupa.csv");

    return;

    std::shared_ptr<Matrix> dupa = engine->db->GetMatrix(1);

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)dupa->expectedBiClusters[0]->cluster1.size(), (uint)dupa->expectedBiClusters[0]->cluster2.size());

    for (uint i = 0; i < dupa->expectedBiClusters[0]->cluster1.size(); ++i)
    {
        for (uint j = 0; j < dupa->expectedBiClusters[0]->cluster2.size(); ++j)
        {
            try
            {
                Amatrix(j, i) = dupa->data(dupa->expectedBiClusters[0]->cluster1[j], dupa->expectedBiClusters[0]->cluster2[i]);
            }
            catch(...)
            {
                qDebug() << "Panic. ACV part errror: " << dupa->expectedBiClusters[0]->cluster1[j] << "," << dupa->expectedBiClusters[0]->cluster2[i];
            }
        }
    }

    QDir dir = QDir::current();

    if (!dir.cd("tmp"))
    {
        dir.mkdir("tmp");
    }

    QString filename = "tmp/test.mat";

    Amatrix.save(filename.toStdString(), arma::arma_ascii);

    //qDebug() << dupa->AverageSpearmansRank(Amatrix);

    //qDebug() << dupa->AverageCorrelationValue(Amatrix);

    //qDebug() << dupa->ScalingMeanSquaredResidue(Amatrix);

    //qDebug() << dupa->MeanSquaredResidue(Amatrix);

    //qDebug() << dupa->Variance(Amatrix);

    //qDebug() << engine->db->GetResults(-1,-1,-1,-1).size();

    //ImportKumalResults(20);
    //ImportKumalResultsNoise(20);
    //ImportKumalResultsNumber(20);
    //ImportKumalResultsOverlap(20);


    //std::vector<int> mats = engine->db->getGroupOfMatrices("rep0");

//    int i = 1;

//    for(int mat : mats)
//    {
//        if (i > 50 && i <= 150)
//        {
//            //std::shared_ptr<Matrix> matData = engine->db->GetMatrix(mat);

//            //RunNMF(mat, matData->expectedBiClusterCount, matData->expectedBiClusterCount, 1, 5);

//            RunConsensus(mat);

//            RunTriclustering(mat);

//            qDebug() << "Done " << i << " from " << mats.size();
//        }

//        i++;
//    }

    //return;

//    std::vector<QString> types;
//    types.push_back("Standard");
//    types.push_back("ByACV");
//    types.push_back("None");
//    types.push_back("ByACVHeuristic");

//    std::vector<std::shared_ptr<BiclusteringObject>> consensus = engine->db->GetResults(-1,-1,(int)Enums::Methods::CONSENSUS,-1);

//    for(QString type : types)
//    {
//        for(std::shared_ptr<BiclusteringObject> single : consensus)
//        {
//            if (single->desc.left(type.size()) == type)
//            {
//                PringInfo(single, type + ".csv");

//                std::vector<int> test = engine->db->GetResultsIDs(*single->dataMatrix->idMatrix);

//                for(int singleTest : test)
//                {
//                    std::vector<std::shared_ptr<BiclusteringObject>> sig = engine->db->GetResults(singleTest,-1,-1,-1);

//                    PringInfo(sig[0], type + ".csv");
//                }
//            }
//        }
//    }
    //return;
//qDebug() << "Mission accomplished!";
//    for(int r=10311; r<=14024; ++r)

//    for(int r=12001; r<=15000; ++r)
//    {
//        std::vector<std::shared_ptr<BiclusteringObject>> sig = engine->db->GetResults(r,-1,-1,-1);

//        PringInfo(sig[0]);
//    }
    return;

    std::vector<int> ids = engine->db->getGroupOfMatrices("rep0");

    //ids.push_back(1);
    //ids.push_back(2);

    for(int id : ids)
    {
        try
        {
            //RunConsensus(id);
        }
        catch(...){}
    }

    for(int id : ids)
    {
        try
        {
            //RunTriclustering(id);
        }
        catch(...){}
    }

    engine->ServeQueue();

    qDebug() << "Mission accomplished!";
    //ExportResults("", 1, 360);

    //DividePowerlogs();

    //RunAllConsensus2(7);

    //RunStepTricluster(7, 5);

    //return;

    //std::vector<int> resultIDs;

    //for(int m = 2; m <= 5; ++m)
    //{
        //resultIDs.push_back(m);
        //std::vector<int> resultIDs = engine->db->GetResultsIDs(m);
        //qDebug() << "Size: " << resultIDs.size();
        //InputForBingo("Bingo" + QString::number(m) + "_Kumal", resultIDs, m);
    //int m=8;
    //}

    //InputForBingo("Bingo1_Final", resultIDs, 1);
    //InputForBingo("Bingo8_Final", resultIDs, 8);
    //}
    //InputForBingo("Bingo4_100", 2771, 3610);

    //CheckSimiliarity();


    //return;
    //QStringList params = mode.split(';');

    //int start = params[0].toInt();
    //int stop = params[1].toInt();
    //int matrixS = params[2].toInt();
    //int matrixE = params[3].toInt();

    //for(int m = matrixS; m <= matrixE; ++m)
    //{
        //RunNMF(m, start, stop, 5, 10);

        //QThreadPool::globalInstance()->waitForDone();

        //RunStepConsensus(m, 2, 5, 1);

        //RunStepTricluster(m, 5);
    //}
    //std::vector<std::shared_ptr<BiclusteringObject>> single1 = engine->db->GetResults(1931, -1, -1, -1);

    //std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(single1[0]);

    //tmpPtr->expectedBiClusterCount = tmpPtr->foundedBiclusters.size();

    //tmpPtr->RebuildBiclusters();

    //setProgressChange(90);
}


void Experimental::CheckSimiliarity()
{
    std::vector<std::shared_ptr<BiclusteringObject>> single1 = engine->db->GetResults(1330, -1, -1, -1);
    std::vector<std::shared_ptr<BiclusteringObject>> single2 = engine->db->GetResults(1331, -1, -1, -1);

    ClassicalHungarian hungarianEngine;

    Array<double> CM = single1[0]->GetCostMatrixForBiclusters(single1[0]->foundedBiclusters, single2[0]->foundedBiclusters, Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

    hungarianEngine.SetCostMatrix(CM, ClassicalHungarian::MunkresFunc::Max);

    double test = hungarianEngine.RunMunkres();

    qDebug() << test / single1[0]->foundedBiclusters.size();
}

void Experimental::InputForBingo(QString file, std::vector<int> resIDs, int matID)
{
    std::vector<std::shared_ptr<BiclusteringObject>> test;

    /*
     *   1,3,7 - Homo Sapiens
     *   2,4 - Rattus norvegicus
     *   5 - Caenorhabditis elegans
     *   6,8 - Mus musculus

     */

    int index = 0;

    for(int r : resIDs)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> single = engine->db->GetResults(r, -1, -1, -1);

        qDebug() << *single[0]->dataMatrix->idMatrix;

        //if (    *single[0]->dataMatrix->idMatrix == matID )
//             && (   single[0]->idMethod == Enums::Methods::CONSENSUS
//                 || single[0]->idMethod == Enums::Methods::TRICLUSTERING
//                 || single[0]->idMethod == Enums::Methods::PLSA
//                 || single[0]->idMethod == Enums::Methods::LEAST_SQUARE_ERROR
//                 || single[0]->idMethod == Enums::Methods::KULLBACK_LIEBER
//                 || single[0]->idMethod == Enums::Methods::NonSmooth_KULLBACK_LIEBER
//                )
//           )
            test.push_back(single[0]);

        qDebug() << QString::number(++index) << " done, " << QString::number(resIDs.size() - index) << " to go. In ensemble: " << QString::number(test.size());
    }

    QFile retVal(file + ".txt");

    retVal.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream out(&retVal);

    //QString path = "D:\\AppData\\KumalData\\Kumal137\\";

    for(std::shared_ptr<BiclusteringObject> result : test)
    {
        int index = 0;

        int foundedBiClusterSize = result->foundedBiclusters.size();

        //result->PostProcessingTask();

        for(std::shared_ptr<Bicluster> bic : result->foundedBiclusters)
        {
            //QString oldName = path + "cluster_" + QString::number(result->idResult) + "_" + QString::number(result->idMethod) + "_" + QString::number(*result->dataMatrix->idMatrix) + "_" + QString::number(foundedBiClusterSize) + "_" + QString::number(index) + "_0.bgo";

            //QString newName = path + "cluster_" + QString::number(result->idResult) + "_" + QString::number(result->idMethod) + "_" + QString::number(*result->dataMatrix->idMatrix) + "_" + QString::number(foundedBiClusterSize) + "_" + QString::number(index) + "_" + QString::number(*bic->ACV) + ".bgo";

            //bool resultRename = QFile::rename(oldName, newName);

            //qDebug() << oldName << " -> " << newName << " = " << resultRename;

            out << "cluster_" + QString::number(result->idResult) + "_" + QString::number(result->idMethod) + "_" + QString::number(*result->dataMatrix->idMatrix) + "_" + QString::number(foundedBiClusterSize) + "_" + QString::number(index++) << "_" << QString::number(*bic->GetFeature(Enums::FeatureType::ACV)) << "\n";

            //out << QString::number(result->idResult) + ";" + QString::number(result->idMethod) + ";" + QString::number(*result->dataMatrix->idMatrix) + ";" + QString::number(foundedBiClusterSize) + ";" + QString::number(index++) << ";" << QString::number(*bic->ACV) << "\n";

            for(int c1 : bic->cluster1)
            {
                out << result->dataMatrix->rowLabels[c1].value << "\n";
                //out << c1 << "\n";
            }

            out << "batch\n";
        }
    }

    qDebug() << "Hurra";

    retVal.close();
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
                        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, currentBic);
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
                std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(res->idResult, cluster1, currentBic);
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

void Experimental::RunTriclustering(int matrix, std::vector<Enums::Methods> methods, QString desc)
{

    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(Enums::Methods method : methods)
    {

        std::vector<std::shared_ptr<BiclusteringObject>> testSingle  = engine->db->GetResults(-1, matrix, method, -1);
        //auto riter = std::remove_if(testSingle.begin(), testSingle.end(), [](std::shared_ptr<BiclusteringObject> r) { return r->idMethod == Enums::Methods::CONSENSUS; });
        //testSingle.erase(riter, testSingle.end());

        if (testSingle.size() == 0)
            continue;
        /*
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
*/
        for(std::shared_ptr<BiclusteringObject> single : testSingle)
            test.push_back(single);
    }

    if (test.size() == 0)
        return;

    //std::vector<MergeType> mt;

    //mt.push_back(Consensus::MergeType::ByACV);
    //mt.push_back(Consensus::MergeType::ByACVHeuristic);
    //mt.push_back(Consensus::MergeType::Standard);

    //for(Consensus::MergeType imt : mt)
    //{
        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1, 0, desc);

        newObject->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

        newObject->dataMatrix->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

        newObject->SetEnsemble(test);

        //newObject->ExtractType = imt;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        //auto res = newObject->Compute(params);

        //engine->db->SaveResult(res);

        engine->AddBiClusteringTask(newObject);
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

        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(test[0]->dataMatrix, -1, 0, "");

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

    ids.push_back(2);
    ids.push_back(3);
    ids.push_back(4);
    ids.push_back(6);
    ids.push_back(7);
    ids.push_back(8);
    ids.push_back(1);
    ids.push_back(5);


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

        //mt.push_back(MergeType::ByACVHeuristic);
        //mt.push_back(MergeType::ByACV);
        //mt.push_back(MergeType::Standard);
        mt.push_back(MergeType::None);

        for(MergeType imt : mt)
        {
            std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1, -1, "");

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

void Experimental::RunSingleNMF(int matrix, Enums::Methods method)
{
    std::shared_ptr<BiclusteringObject> newObject;

    std::shared_ptr<Matrix> vMatrix = engine->db->GetMatrix(matrix);

    switch(method)
    {
        case Enums::PLSA:
            newObject = std::make_shared<PLSA>(vMatrix);
            break;
        case Enums::LEAST_SQUARE_ERROR:
            newObject = std::make_shared<LSE>(vMatrix);
            break;
        case Enums::KULLBACK_LIEBER:
            newObject = std::make_shared<KullbackLeibler>(vMatrix);
            break;
        case Enums::NonSmooth_KULLBACK_LIEBER:
            newObject = std::make_shared<nsKullbackLeibler>(vMatrix, 0.5);
            break;
    }

    newObject->expectedBiClusterCount = vMatrix->expectedBiClusterCount;

    newObject->dataMatrix->expectedBiClusterCount = vMatrix->expectedBiClusterCount;

    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

    params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

    engine->AddBiClusteringTask(newObject);
}

void Experimental::RunNMF(int matrix, int start, int stop, int step, uint rep)
{
    for(int mat = matrix; mat <=matrix; ++mat)
    {
        std::shared_ptr<Matrix> vMatrix = engine->db->GetMatrix(mat);

        for(uint s = stop; s >= start; s = s - step)
        {
            for(uint m = 0; m < 4; ++m)
            {
                int currRep = rep - engine->db->GetResultsCount(-1, mat, m, s);

                for(uint r = 0; r < currRep; r++)
                {
                    std::shared_ptr<BiclusteringObject> newObject;

                    switch(m)
                    {
                        case Enums::PLSA:
                            newObject = std::make_shared<PLSA>(vMatrix);
                            break;
                        case Enums::LEAST_SQUARE_ERROR:
                            newObject = std::make_shared<LSE>(vMatrix);
                            break;
                        case Enums::KULLBACK_LIEBER:
                            newObject = std::make_shared<KullbackLeibler>(vMatrix);
                            break;
                        case Enums::NonSmooth_KULLBACK_LIEBER:
                            newObject = std::make_shared<nsKullbackLeibler>(vMatrix, 0.5);
                            break;
                    }

                    newObject->expectedBiClusterCount = s;

                    newObject->dataMatrix->expectedBiClusterCount = s;

                    std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

                    params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

                    params.emplace_back(Enums::MethodsParameters::SaveToLocalFile, std::make_shared<QString>("E:\\pfoszner\\results\\batch\\" + QString::number(mat) + "_" + QString::number(s) + "_" + QString::number(m)));

                    engine->AddBiClusteringTask(newObject);

                    qDebug() << "Matrix " << mat << " Run Bi-cluster: " << s << ", Repetition: " << r;
                }
            }
        }
    }

    //engine->ServeQueue();
}

void Experimental::RunAllConsensus2(int idMatrix)
{
    std::vector<std::shared_ptr<BiclusteringObject>> test;

        uint start = 5;

        for(uint s = start; s <= 100; s = s + 5)
        {
            test = engine->db->GetResults(-1, idMatrix, -1, s);

            auto riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return r->idMethod == Enums::Methods::CONSENSUS || r->idMethod == Enums::Methods::TRICLUSTERING || r->idResult < 100; });
            test.erase(riter, test.end());

            double average = 0;

            for(std::shared_ptr<BiclusteringObject> dupa : test)
            {
                average += dupa->foundedBiclusters.size();
            }

            int bicCount = (int)(average / test.size());

            qDebug() << "Matrix: (id=" << idMatrix << ") " << test[0]->dataMatrix->name << " Average: " << bicCount << " Step bicluster: " << s;

            std::vector<MergeType> mt;

            mt.push_back(MergeType::ByACV);
            mt.push_back(MergeType::ByACVHeuristic);
            mt.push_back(MergeType::Standard);

            for(MergeType imt : mt)
            {
                std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1, -1, "");

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

void Experimental::RunStepConsensus(int matrix, int start, int stop, int step)
{
    std::shared_ptr<Matrix> vMatrix = engine->db->GetMatrix(matrix);

    for(uint s = start; s <= stop; s = s + step)
    {
        std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(vMatrix, -1, 0, "");

        std::vector<std::shared_ptr<BiclusteringObject>> test  = engine->db->GetResults(-1, matrix, -1, s);

        auto riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return !(r->idResult > 128); });
        test.erase(riter, test.end());

        riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return (r->idMethod == Enums::Methods::CONSENSUS || r->idMethod == Enums::Methods::TRICLUSTERING); });
        test.erase(riter, test.end());

        qDebug() << "Results to merge: " << test.size();

        newObject->expectedBiClusterCount = s;

        newObject->dataMatrix->expectedBiClusterCount = s;

        newObject->SetEnsemble(test);

        newObject->ExtractType = MergeType::ByACVHeuristic;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);

        qDebug() << "Done s = " << s;
    }

    //engine->ServeQueue();
}

void Experimental::RunStepTricluster(int matrix, int start)
{
    std::shared_ptr<Matrix> vMatrix = engine->db->GetMatrix(matrix);

    //mt.push_back(Consensus::MergeType::ByACV);
    //mt.push_back(Consensus::MergeType::ByACVHeuristic);
    //mt.push_back(MergeType::Standard);

    for(uint s = start; s <= 100; s = s + 5)
    {
        std::shared_ptr<TriClustering> newObject = std::make_shared<TriClustering>(vMatrix, -1, 0, "");

        std::vector<std::shared_ptr<BiclusteringObject>> test  = engine->db->GetResults(-1, matrix, -1, s);

        auto riter = std::remove_if(test.begin(), test.end(), [](std::shared_ptr<BiclusteringObject> r){ return r->idMethod == Enums::Methods::CONSENSUS || r->idMethod == Enums::Methods::TRICLUSTERING || r->idResult < 100; });
        test.erase(riter, test.end());

        qDebug() << "Results to merge: " << test.size();

//        for(std::shared_ptr<BiclusteringObject> item : test)
//        {
//            std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(item);

//            tmpPtr->foundedBiclusters.clear();

//            tmpPtr->expectedBiClusterCount = s;

//            tmpPtr->dataMatrix->expectedBiClusterCount = s;

//            tmpPtr->RebuildBiclusters();
//        }

        newObject->expectedBiClusterCount = s;

        newObject->dataMatrix->expectedBiClusterCount = s;

        newObject->SetEnsemble(test);

        //newObject->ExtractType = MergeType::ByACVHeuristic;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        auto res = newObject->Compute(params);

        engine->db->SaveResult(res);

        qDebug() << "Done s = " << s;
    }

    //engine->ServeQueue();
}

void Experimental::RunConsensus(int matrix, std::vector<Enums::Methods> methods, QString desc)
{


    //methods.push_back(Enums::Methods::PLSA);
    //methods.push_back(Enums::Methods::LEAST_SQUARE_ERROR);
    //methods.push_back(Enums::Methods::KULLBACK_LIEBER);
    //methods.push_back(Enums::Methods::NonSmooth_KULLBACK_LIEBER);


    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(Enums::Methods method : methods)
    {

        std::vector<std::shared_ptr<BiclusteringObject>> testSingle  = engine->db->GetResults(-1, matrix, method, -1);

        for(std::shared_ptr<BiclusteringObject> single : testSingle)
        {
            test.push_back(single);
        }

        if (test.size() == 0)
            return;
/*
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
*/
    }
    std::vector<MergeType> mt;

    mt.push_back(MergeType::ByACV);
    //mt.push_back(MergeType::ByACVHeuristic);
    //mt.push_back(MergeType::Standard);
    //mt.push_back(MergeType::None);
    //mt.push_back(MergeType::GainFunction);

    std::vector<QString> descs;

    descs.push_back("ByACV");
    //descs.push_back("ByACVHeuristic");
    //descs.push_back("Standard");
    //descs.push_back("None");
    //descs.push_back("GainFunction");

    std::vector<Enums::FeatureType> ft;

    ft.push_back(Enums::FeatureType::ACV);
    //ft.push_back(Enums::FeatureType::ASR);
    //ft.push_back(Enums::FeatureType::MSR);
    //ft.push_back(Enums::FeatureType::SMSR);
    //ft.push_back(Enums::FeatureType::Variance);


    std::vector<QString> descsFeat;

    descsFeat.push_back("ACV");
    descsFeat.push_back("ASR");
    descsFeat.push_back("MSR");
    descsFeat.push_back("SMSR");
    descsFeat.push_back("Variance");

    int f = 0;

    for(Enums::FeatureType ift : ft)
    {
        int d = 0;

        for(MergeType imt : mt)
        {
            std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(test[0]->dataMatrix, -1, 0, desc + "_" + descs[d++] + "_" + descsFeat[f]);

            //std::shared_ptr<Consensus> newObject = std::make_shared<Consensus>(engine->CurrentVmatrix, -1);

            newObject->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

            newObject->dataMatrix->expectedBiClusterCount = newObject->dataMatrix->expectedBiClusterCount;

            newObject->SetEnsemble(test);

            newObject->ExtractType = imt;

            newObject->qualityMeasure = ift;

            std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

            params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

            //auto res = newObject->Compute(params);

            //engine->db->SaveResult(res);

            if (imt == MergeType::GainFunction)
            {

                    newObject->penaltyForGainFunction = f + 1;
                    newObject->desc += "_" + QString::number(f + 1);

            }


            engine->AddBiClusteringTask(newObject);
        }

        f++;
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

        //RunConsensus(1);
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

        //RunConsensus(1);
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

                QByteArray rawData = newObject->Serialize(true);

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


                    bicObject = std::make_shared<BiclusteringObject>(dataMatrix, methodID, -1, -1, "Kumal Import");
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

                    std::shared_ptr<Bicluster> fb = std::make_shared<Bicluster>(-1, cluster1, cluster2);

                    bicObject->foundedBiclusters.push_back(fb);
                }
            }
        }
    }
}


void Experimental::Muszki()
{
    Matrix test("/mnt/D/tmp/est_counts.vmatrix");

    engine->db->SaveLabels(test.rowLabels, 1);
    engine->db->SaveLabels(test.columnLabels, 1);

    Matrix test2("/mnt/D/tmp/tmp.vmatrix");

    engine->db->SaveLabels(test2.rowLabels, 2);
    engine->db->SaveLabels(test2.columnLabels, 2);

    //engine->db->SaveMatrix(test.data, test.name, test.group, Enums::MatrixType::V, -1);
}

void Experimental::PLATResults(QString folder, int startId, int stopID, int classNum)
{

    QFile textFile("/mnt/D/AppData/owncloud/Praca/tmp/labels.csv");

    textFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream textStream(&textFile);

    std::vector<QString> sex;
    std::vector<QString> strim;
    std::vector<QString> env;
    std::vector<QString> strimSex;

    while (true)
    {
        QString line = textStream.readLine();
        if (line.isNull())
            break;
        else
        {
            QStringList tmp = line.split(';');

            strim.push_back(tmp[1]);
            sex.push_back(tmp[2]);
            env.push_back(tmp[3]);
            strimSex.push_back(tmp[4]);
        }
    }

    std::vector<std::shared_ptr<BiclusteringObject>> test;

    for(int r = startId; r <= stopID; ++r)
    {
        std::vector<std::shared_ptr<BiclusteringObject>> single = engine->db->GetResults(r, -1, -1, -1);

        test.push_back(single[0]);
    }

    for(std::shared_ptr<BiclusteringObject> result : test)
    {
        QDir dir = QDir::current();

        if (!dir.cd(folder))
        {
            dir.mkdir(folder);
        }

        QFile retVal(folder + "/result_" + QString::number(result->idResult) + "_" + QString::number(result->idMethod) + "_" + QString::number(*result->dataMatrix->idMatrix) + ".txt");

        retVal.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&retVal);

        for(std::shared_ptr<Bicluster> bic : result->foundedBiclusters)
        {
            QHash<QString, int> percentage;

            //out << "Bicluster " << ++index << ". Average corelation value: " << *bic->ACV << "\n";

            //out << "Cluster1:\n";

            //for(int c1 : bic->cluster1)
            //{
                //out << result->dataMatrix->rowLabels[c1].value << "\n";
                //out << c1 << "\n";
            //}

            //out << "\nCluster2:\n";

            for(int c2 : bic->cluster2)
            {
                QString label;

                //out << result->dataMatrix->columnLabels[c2].value << "\n";
                if (classNum == 1)
                {
                    label = sex[c2];
                }
                else if (classNum == 2)
                {
                    label = strim[c2];
                }
                else if (classNum == 3)
                {
                    label = env[c2];
                }
                else if (classNum == 4)
                {
                    label = strimSex[c2];
                }

                //out << sex[c2] << "\n";

                if (percentage.contains(label))
                {
                    percentage[label]++;
                }
                else
                {
                    percentage[label] = 1;
                }
                //out << c2 << "\n";
            }

            for(QString key : percentage.keys())
            {
                out << key << ": " << percentage[key] << " - " << ((double)percentage[key] / bic->cluster2.size()) << "\n";
            }

            out << "\n\n";
        }

        retVal.close();

        QFile labels(folder + "/genes_" + QString::number(*result->dataMatrix->idMatrix) + ".txt");

        labels.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream outL(&labels);

        for(Label item : result->dataMatrix->rowLabels)
        {
            outL << item.value << "\n";
        }

        labels.close();
    }


    qDebug() << "Huuuurraaaa";
}


void Experimental::ExportResults(QString folder, int startId, int stopID)
{
        QFile retVal("results.csv");

        retVal.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&retVal);


        std::vector<std::shared_ptr<BiclusteringObject>> test;// = engine->db->GetResults(-1, -1, -1, -1);;

        for(int r = startId; r <= stopID; ++r)
        {
            std::vector<std::shared_ptr<BiclusteringObject>> single = engine->db->GetResults(r, -1, -1, -1);


            std::shared_ptr<BiclusteringObject> result = single[0];
        //    test.push_back(single[0]);
        //}

        //for(std::shared_ptr<BiclusteringObject> result : test)
        //{
            result->PostProcessingTask();

//            QDir dir = QDir::current();

//            if (!dir.cd(folder))
//            {
//                dir.mkdir(folder);
//            }

            int index = 0;

            for(std::shared_ptr<Bicluster> bic : result->foundedBiclusters)
            {
                //QFile retVal(folder + "/result_" + QString::number(result->idResult) + "_" + QString::number(result->idMethod) + "_" + QString::number(*result->dataMatrix->idMatrix) + "_" + QString::number(index++) + ".txt");

                //retVal.open(QIODevice::WriteOnly | QIODevice::Text);

                //QTextStream out(&retVal);

                //out << "Bicluster " << ++index << ". Average corelation value: " << *bic->ACV << "\n";

                qDebug() << *result->dataMatrix->idMatrix << ";" << result->idResult << ";" << result->idMethod << ";Bicluster " << index << ";" << *bic->GetFeature(Enums::FeatureType::ACV) << "\n";

                out << *result->dataMatrix->idMatrix << ";" << result->idResult << ";" << result->idMethod << ";Bicluster " << ++index << ";" << *bic->GetFeature(Enums::FeatureType::ACV) << "\n";

                //out << "Cluster1:\n";

                //for(int c1 : bic->cluster1)
                //{
                    //out << result->dataMatrix->rowLabels[c1].value << "\n";
                    //out << c1 << "\n";
                //}

                //out << "\nCluster2:\n";

                //for(int c2 : bic->cluster2)
                //{
                    //out << result->dataMatrix->columnLabels[c2].value << "\n";
                    //out << c2 << "\n";
                //}

                //out << "\n\n";

                //retVal.close();
            }

//            QFile labels(folder + "/genes_" + QString::number(*result->dataMatrix->idMatrix) + ".txt");

//            labels.open(QIODevice::WriteOnly | QIODevice::Text);

//            QTextStream outL(&labels);

//            for(Label item : result->dataMatrix->rowLabels)
//            {
//                outL << item.value << "\n";
//            }

//            labels.close();
        }

        retVal.close();

        qDebug() << "Huuuurraaaa";
}

void Experimental::DividePowerlogs()
{
    QFile file("log.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);

        long start = -1;
        long stop = -1;
        long prevTime = -1;
        int id = -1;

        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();

            QStringList tmp = line.split(';');

            int tmpID = tmp[1].toInt();;

            if (id == -1)
            {
                id = tmpID;

                start = tmp[0].toLong() - 153;

                qDebug() << "New id found: " << id << " start time: " << start;
            }
            else if (tmpID != id)
            {
                stop = prevTime;

                qDebug() << "Analysis of id " << id << " time: " << start << " to " << stop;

                QFile powerfile("powerlog.log");

                powerfile.open(QIODevice::ReadOnly);

                QTextStream powerin(&powerfile);


                QFile retVal(QString::number(id) + ".log");

                retVal.open(QIODevice::WriteOnly | QIODevice::Text);

                QTextStream out(&retVal);

                out << powerin.readLine();

                while(!powerin.atEnd()) {
                    QString powerline = powerin.readLine().trimmed();

                    QStringList powerTmp = powerline.split(',');

                    double powertime = powerTmp[0].toDouble();

                    if (powertime >= start && powertime <= stop)
                    {
                        out << powerline << "\n";
                    }
                }

                retVal.close();

                powerfile.close();

                id = tmpID;

                start = tmp[0].toLong() - 153;

                qDebug() << "New id found: " << id << " start time: " << start;
            }

            prevTime = tmp[0].toLong() - 153;
        }

        stop = prevTime;

        qDebug() << "Analysis of id " << id << " time: " << start << " to " << stop;

        QFile powerfile2("powerlog.log");

        powerfile2.open(QIODevice::ReadOnly);

        QTextStream powerin2(&powerfile2);

        QFile retVal2(QString::number(id) + ".log");

        retVal2.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out2(&retVal2);

        out2 << powerin2.readLine();

        while(!powerin2.atEnd()) {
            QString powerline2 = powerin2.readLine().trimmed();

            QStringList powerTmp2 = powerline2.split(',');

            double powertime2 = powerTmp2[0].toDouble();

            if (powertime2 >= start && powertime2 <= stop)
            {
                out2 << powerline2 << "\n";
            }
        }

        retVal2.close();

        powerfile2.close();

        file.close();
    }

    qDebug() << "Done";
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

arma::mat Experimental::GetSimpleMatrix1()
{
    arma::arma_rng::set_seed_random();

    int N = 500;

    arma::mat gtData = arma::mat(N, N, arma::fill::randu);
    //arma::mat gtDataOrig = arma::mat(N, N, arma::fill::randu);

    for(int k = 0; k < 5; ++k)
    {
        for(int i=(k*100)+10; i < (k*100)+90; ++i)
        {
            for(int j=(k*100)+10; j < (k*100)+90; ++j)
            {
                gtData(i,j) = 2.0;
                //gtDataOrig(i, j) = gtData(i,j);
            }
        }
    }

    return gtData;
}

void Experimental::CheckNMFBiclusNumberSearch()
{
    arma::arma_rng::set_seed_random();


    arma::mat gtData = GetSimpleMatrix1();

    arma::mat currentData = arma::mat(gtData.n_rows, gtData.n_cols, arma::fill::randu);

    arma::mat gtDataOrig = arma::mat(gtData);

    Matrix gt(-1, gtData);
    Matrix current(-1, currentData);

    arma::mat powComponent = gtData - currentData;

    qDebug() << "Initial distance: " << arma::accu(powComponent % powComponent);

    gt.WriteAsImage("gt0.png");
    current.WriteAsImage("current0.png");

    int iteration = 0;

    while(iteration < 10)
    {
    //nsKullbackLeibler
        std::shared_ptr<BiclusteringObject> newObject;
        std::shared_ptr<Matrix> m = std::make_shared<Matrix>(gt);
        newObject = std::make_shared<KullbackLeibler>(m);

        newObject->expectedBiClusterCount = 10;

        newObject->dataMatrix->expectedBiClusterCount = 10;

        std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

        params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(newObject->dataMatrix->expectedBiClusterCount));

        newObject->Compute(params);

        int ix = 0;
        double simi = -1;

        for(int i = 0; i < newObject->foundedBiclusters.size(); ++i)
         {
                 arma::mat test = arma::mat(currentData);

                 for(int c1 : newObject->foundedBiclusters[i]->cluster1)
                 {
                     for(int c2 : newObject->foundedBiclusters[i]->cluster2)
                     {
                         test(c1, c2) = gtDataOrig(c1, c2);
                     }
                 }

                 arma::mat powComponent = gtDataOrig - test;

                 if (simi < 0 || simi > arma::accu(powComponent % powComponent))
                 {
                     ix = i;
                     simi = arma::accu(powComponent % powComponent);
                 }
         }

         for(int c1 : newObject->foundedBiclusters[ix]->cluster1)
         {
             for(int c2 : newObject->foundedBiclusters[ix]->cluster2)
             {
                 current.data(c1, c2) = gtDataOrig(c1, c2);
                 gt.data(c1, c2) = fRand(0, 1);
             }
         }

        iteration++;

        arma::mat powComponent = gtDataOrig - current.data;

        simi = arma::accu(powComponent % powComponent);

        qDebug() << iteration << " iteration distance: " << simi;

        gt.WriteAsImage("gt" + QString::number(iteration) + ".png");
        current.WriteAsImage("current" + QString::number(iteration) + ".png");
    }

    qDebug() << "Done";
}
