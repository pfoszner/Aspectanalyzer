#include "biclusteringobject.h"

BiclusteringObject::BiclusteringObject(std::shared_ptr<Matrix>& dataMatrix)
{
    //this->dataMatrix = std::make_shared<Matrix>(dataMatrix);
    this->dataMatrix = dataMatrix;
}

Array<double> BiclusteringObject::GetCostMatrixForBiclusters(const std::vector<std::shared_ptr<Bicluster>>& original, const std::vector<std::shared_ptr<Bicluster>>& computed, Enums::BiclusterCompareMode mode, Enums::SimilarityMethods simMethod)
{
    Array<double> CostMatrix(original.size(), computed.size());

    for(uint i = 0; i < original.size(); ++i)
    {
        for(uint j = 0; j < computed.size(); ++j)
        {
            try
            {
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
            }
            catch (...)
            {
                //Logger.Log("Error in comparing Biclusters: " + ex.Message, LogTypes.Error);
                //throw new Exception("Error in comparing Biclusters: " + ex.Message);
            }
        }
    }

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
