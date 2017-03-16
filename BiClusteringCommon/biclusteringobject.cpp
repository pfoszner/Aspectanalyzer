#include "biclusteringobject.h"

BiclusteringObject::BiclusteringObject(std::shared_ptr<Matrix>& dataMatrix)
{
    this->dataMatrix = std::make_shared<Matrix>(*dataMatrix);
    //this->dataMatrix = dataMatrix;
}

Array<double> BiclusteringObject::GetCostMatrixForBiclusters(const std::vector<std::shared_ptr<Bicluster>>& original, const std::vector<std::shared_ptr<Bicluster>>& computed, Enums::BiclusterCompareMode mode, Enums::SimilarityMethods simMethod)
{
    uint size = original.size();

    if (computed.size() > size)
        size = computed.size();

    Array<double> CostMatrix(size, size);

    for(uint i = 0; i < size; ++i)
    {
        for(uint j = 0; j < size; ++j)
        {
            try
            {
                if (i >= original.size() || j >= computed.size())
                {
                    CostMatrix[i][j] = 0.0;
                }
                else if (original.size() == 0 || computed.size() == 0)
                {
                    CostMatrix[i][j] = 0.0;
                }
                else
                {
                    CostMatrixWorker *st = new CostMatrixWorker(&CostMatrix[i][j], original[i], computed[j], mode, simMethod);

                    QThreadPool::globalInstance()->start(st);

                    /*
                    switch (mode)
                    {
                        case Enums::BiclusterCompareMode::Both:
                            CostMatrix[i][j] = original[i]->Compare(computed[j], simMethod);
                            break;
                        case Enums::BiclusterCompareMode::First:
                            CostMatrix[i][j] = original[i]->CompareFirst(computed[j], simMethod);
                            break;
                        case Enums::BiclusterCompareMode::Second:
                            CostMatrix[i][j] = original[i]->CompareSecond(computed[j], simMethod);
                            break;
                    }
                    */
                }
            }
            catch (...)
            {
                //Logger.Log("Error in comparing Biclusters: " + ex.Message, LogTypes.Error);
                //throw new Exception("Error in comparing Biclusters: " + ex.Message);
            }
        }
    }

    QThreadPool::globalInstance()->waitForDone();

    return CostMatrix;
}

void BiclusteringObject::PostProcessingTask()
{
    if (foundedBiclusters.size() == 0)
    {
        qDebug() << "Panic! Panic! Panic!";
    }

    if (dataMatrix->expectedBiClusters.size() > 0)
    {
        Array<double> CM = GetCostMatrixForBiclusters(dataMatrix->expectedBiClusters, foundedBiclusters, Enums::BiclusterCompareMode::Both, Enums::SimilarityMethods::JaccardIndex);

        //Array<double> CM(dataMatrix->expectedBiClusters.size(), foundedBiclusters.size());

        ClassicalHungarian Classis;

        Classis.SetCostMatrix(CM, ClassicalHungarian::MunkresFunc::Max);

        double Value = Classis.RunMunkres();

        for (uint i = 0; i < dataMatrix->expectedBiClusters.size(); ++i)
        {
            for (uint j = 0; j < foundedBiclusters.size(); ++j)
            {
                if (Classis.M[i][j] == 1)
                {
                    double simi = Classis.C_orig[i][j];
                    foundedBiclusters[j]->similarity = std::make_shared<double>(simi);
                    break;
                }
            }
        }

        features.push_back(FeatureResult(Enums::FeatureType::AverageSimilarity, Value / foundedBiclusters.size(), 0));

        //delete Classis;
    }

    std::shared_ptr<double> AverageAVC = std::make_shared<double>(0);

    if (foundedBiclusters.size() > 0)
    {
        for(uint i = 0; i < foundedBiclusters.size(); ++i)
        {
            if (foundedBiclusters[i]->cluster1.size() > 0 && foundedBiclusters[i]->cluster2.size() > 0)
            {
                foundedBiclusters[i]->ACV = dataMatrix->AverageCorrelationValue(foundedBiclusters[i]->cluster1, foundedBiclusters[i]->cluster2);
                *AverageAVC += *foundedBiclusters[i]->ACV;
            }
            else
                foundedBiclusters[i]->ACV = nullptr;
        }
    }

    if (AverageAVC != nullptr)
    {
        features.push_back(FeatureResult(Enums::FeatureType::AverageACV, *AverageAVC / foundedBiclusters.size(), 0));
    }
}

std::shared_ptr<BiclusteringObject> BiclusteringObject::Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    std::shared_ptr<BiclusteringObject> null;

    params.clear();

    return null;
}

void BiclusteringObject::ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    params.clear();
}

void BiclusteringObject::GenerateARFFFile(QString path, int dim, std::vector<int> indexes)
{
    if (indexes.size() == 0)
    {
        if (dim == 0)
        {
            for(uint i = 0; i < dataMatrix->data.n_rows; ++i)
            {
                indexes.push_back(i);
            }
        }
        else
        {
            for(uint i = 0; i < dataMatrix->data.n_cols; ++i)
            {
                indexes.push_back(i);
            }
        }
    }

    QFile retVal(path);

    retVal.open(QFile::WriteOnly | QFile::Text);

    QTextStream out(&retVal);

    out << "@RELATION biclustering\n\n";

    for(uint i = 0; i < indexes.size(); ++i)
    {
        QString value = dataMatrix->columnLabels[indexes[i]].value;

        if (dim == 0)
            value = dataMatrix->rowLabels[indexes[i]].value;

        if (value.size() == 0)
            value = "empty";

        out << "@ATTRIBUTE " << value << " numeric\n";
    }

    QList<QString> distinstClass;

    for(uint i = 0; i < dataMatrix->classLabels.size(); ++i)
    {
        if (dim == 1 && dataMatrix->classLabels[i].idLabelType == Enums::LabelType::RowClassLabel)
        {
            if (!distinstClass.contains(dataMatrix->classLabels[i].value))
            {
                distinstClass.append(dataMatrix->classLabels[i].value);
            }
        }
        else if (dim == 0 && dataMatrix->classLabels[i].idLabelType == Enums::LabelType::ColumnClassLabel)
        {
            if (!distinstClass.contains(dataMatrix->classLabels[i].value))
            {
                distinstClass.append(dataMatrix->classLabels[i].value);
            }
        }
    }

    out << "@ATTRIBUTE class {'";

    for(int i = 0; i < distinstClass.size(); ++i)
    {
        out << distinstClass[i] << "'";

        if (i == distinstClass.size() - 1)
            out << "}\n";
        else
            out << ",'";
    }

    out << "@DATA\n";

    if (dim == 1)
    {
        for(uint r = 0; r < dataMatrix->data.n_rows; ++r)
        {
            for(uint c = 0; c < indexes.size(); ++c)
            {
                out << QString::number(dataMatrix->data(r,indexes[c])).replace(",",".") << ",";
            }

            auto iter = std::find_if(dataMatrix->classLabels.begin(), dataMatrix->classLabels.end(), [r](Label search) { return search.idLabelType == Enums::LabelType::RowClassLabel && search.indexNbr == r; });

            out << "\"" << iter->value << "\"\n";
        }
    }

    retVal.close();
}
