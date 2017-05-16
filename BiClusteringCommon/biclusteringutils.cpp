#include "biclusteringutils.h"

BiClusteringUtils::BiClusteringUtils()
{

}

Array<double> BiClusteringUtils::GetCostMatrixForBiclusters(std::vector<std::shared_ptr<Bicluster>>& original, std::vector<std::shared_ptr<Bicluster>>& computed, Enums::BiclusterCompareMode mode, Enums::SimilarityMethods simMethod)
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
