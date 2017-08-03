#include "biclusteringobject.h"

static inline QByteArray IntToArray(qint32 source);
static inline QByteArray DoubleToArray(double source);
static inline qint32 ArrayToInt(QByteArray source);
static inline double ArrayToDouble(QByteArray source);

BiclusteringObject::BiclusteringObject(std::shared_ptr<Matrix>& dataMatrix)
{
    this->dataMatrix = std::make_shared<Matrix>(*dataMatrix);
    //this->dataMatrix = dataMatrix;
}

BiclusteringObject::BiclusteringObject(QByteArray deserialize)
{

    idMethod = ArrayToInt(deserialize.mid(0, 4));
    deserialize.remove(0, 4);

    //    int idResult = -1;

        idResult = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

    //    int idMethod = -1;
        idMethod = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

    //    int expectedBiClusterCount;

        expectedBiClusterCount = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

    //    double time_spent;
        time_spent = ArrayToDouble(deserialize.mid(0, 8));
        deserialize.remove(0, 8);

    //    QString sourceAddress = "";
        int addresLen = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

        sourceAddress = QString::fromUtf8(deserialize.mid(0, addresLen));

    //    std::shared_ptr<Matrix> dataMatrix; not nedeed becouse of idMethod
    //    std::vector<std::shared_ptr<Bicluster>> foundedBiclusters;

        int bicLen = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

        for(int b = 0; b < bicLen; ++b)
        {
            std::vector<int> clust2;
            std::vector<int> clust1;

            int c1Len = ArrayToInt(deserialize.mid(0, 4));
            deserialize.remove(0, 4);

            for(int c = 0; c < c1Len; ++c)
            {
                clust1.push_back(ArrayToInt(deserialize.mid(0, 4)));
                deserialize.remove(0, 4);
            }

            int c2Len = ArrayToInt(deserialize.mid(0, 4));
            deserialize.remove(0, 4);

            for(int c = 0; c < c2Len; ++c)
            {
                clust2.push_back(ArrayToInt(deserialize.mid(0, 4)));
                deserialize.remove(0, 4);
            }

            std::shared_ptr<Bicluster> bic = std::make_shared<Bicluster>(-1, clust1, clust2, nullptr, nullptr);
            foundedBiclusters.push_back(bic);
        }


    //    std::vector<FeatureResult> features;

        int fLen = ArrayToInt(deserialize.mid(0, 4));
        deserialize.remove(0, 4);

        for(int f = 0; f < fLen; ++f)
        {
            Enums::FeatureType type;
            double value;
            int indexNbr;

            deserialize.remove(0, 8);

            indexNbr = ArrayToInt(deserialize.mid(0, 4));
            deserialize.remove(0, 4);

            type = (Enums::FeatureType)ArrayToInt(deserialize.mid(0, 4));
            deserialize.remove(0, 4);

            value = ArrayToDouble(deserialize.mid(0, 8));
            deserialize.remove(0, 8);

            features.emplace_back(type, value, indexNbr);
        }
}

QByteArray BiclusteringObject::Serialize()
{
    QByteArray buffer;

    buffer.append(IntToArray(*this->dataMatrix->idMatrix));
//    int idResult = -1;
    buffer.append(IntToArray(this->idResult));
//    int idMethod = -1;
    buffer.append(IntToArray(this->idMethod));
//    int expectedBiClusterCount;
    buffer.append(IntToArray(this->expectedBiClusterCount));
//    double time_spent;
    buffer.append(DoubleToArray(this->time_spent));
//    QString sourceAddress = "";
    buffer.append(IntToArray(this->sourceAddress.length()));
    buffer.append(this->sourceAddress.toUtf8());

//    std::shared_ptr<Matrix> dataMatrix; not nedeed becouse of idMethod
//    std::vector<std::shared_ptr<Bicluster>> foundedBiclusters;

    buffer.append(IntToArray(this->foundedBiclusters.size()));

    for(std::shared_ptr<Bicluster> bic : this->foundedBiclusters)
    {
        buffer.append(IntToArray(bic->cluster1.size()));

        for(int c1 : bic->cluster1)
        {
            buffer.append(IntToArray(c1));
        }

        buffer.append(IntToArray(bic->cluster2.size()));

        for(int c2 : bic->cluster2)
        {
            buffer.append(IntToArray(c2));
        }
    }

//    std::vector<FeatureResult> features;

    buffer.append(IntToArray(this->features.size()));

    for(FeatureResult f : this->features)
    {
        buffer.append(IntToArray(f.idFeature));
        buffer.append(IntToArray(f.idResult));
        buffer.append(IntToArray(f.indexNbr));
        buffer.append(IntToArray(f.type));
        buffer.append(DoubleToArray(f.value));
    }

    return buffer;
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

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

QByteArray DoubleToArray(double source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

double ArrayToDouble(QByteArray source)
{
    double temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
