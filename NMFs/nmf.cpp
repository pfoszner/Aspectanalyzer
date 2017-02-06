#include "nmf.h"

NMF::NMF(std::shared_ptr<Matrix>& data) : BiclusteringObject(data), n(data->data.n_cols), p(data->data.n_rows)
{

}

NMF::NMF(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult, double time)
 : BiclusteringObject(Vmatrix, Method, IdResult, time), n(Vmatrix->data.n_cols), p(Vmatrix->data.n_rows)
{

}

double NMF::DivernegceValue()
{
    return 0.0;
}

void NMF::UpdateStep()
{
}

void NMF::PreProcessing()
{
    //BiclusteringObject test(Matrix(std::vector<QString>(0)));
}

void NMF::CheckNonNegativity()
{
    double min = this->dataMatrix->data.min();
    if (min < 0)
        this->dataMatrix->data = this->dataMatrix->data - min;
}

void NMF::ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    for (auto &param : params)
    {
        switch (std::get<0>(param))
        {
            case Enums::MaxMunberOfSteps:
            {
                int* m = reinterpret_cast<int*>(std::get<1>(param).get());
                if (m != nullptr) {
                    maxNumberOfSteps = *m;
                }
                break;
            }
            case Enums::NumberOfBiClusters:
            {
                int* m = reinterpret_cast<int*>(std::get<1>(param).get());
                if (m != nullptr) {
                    expectedBiClusterCount = *m;
                }
                break;
            }
            case Enums::ExMethod:
            {
                Enums::ExtractingMethod* m = reinterpret_cast<Enums::ExtractingMethod*>(std::get<1>(param).get());
                if (m != nullptr) {
                    exMethod = *m;
                }
                break;
            }
            case Enums::Theta:
            {
                double* m = reinterpret_cast<double*>(std::get<1>(param).get());
                if (m != nullptr) {
                    theta = *m;
                }
                break;
            }
            case Enums::Supervised:
            {
                bool* m = reinterpret_cast<bool*>(std::get<1>(param).get());
                if (m != nullptr) {
                    supervisedDim = *m;
                }
                break;
            }
            case Enums::Multithreading:
            {
                //TODO

                break;
            }
        }
    }
}

void NMF::InitializateFirstValues()
{
    srand (time(NULL));

    std::vector<double> colSum (expectedBiClusterCount);

    std::fill(colSum.begin(), colSum.end(), 0);

    WMatrix = arma::zeros<arma::mat>(p, expectedBiClusterCount);

    for (int i = 0; i < p; ++i)
    {
        for (int j = 0; j < expectedBiClusterCount; ++j)
        {
            WMatrix(i, j) = 100.0 * ( (double)rand() / (double)RAND_MAX );
            colSum[j] += WMatrix(i, j);
        }
    }

    //Normalize Columns in matrix W
    for (int i = 0; i < p; ++i)
    {
        for (int j = 0; j < expectedBiClusterCount; ++j)
        {
            WMatrix(i, j) /= colSum[j];
        }
    }

    std::fill(colSum.begin(), colSum.end(), 0);

    HMatrix = arma::zeros<arma::mat>(expectedBiClusterCount, n);

    for (int i = 0; i < expectedBiClusterCount; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            HMatrix(i, j) = 100.0 * ( (double)rand() / (double)RAND_MAX );
            colSum[i] += HMatrix(i, j);
        }
    }

    //Normalize Columns in matrix H
    for (int i = 0; i < expectedBiClusterCount; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            HMatrix(i, j) /= colSum[i];
        }
    }
}


std::shared_ptr<BiclusteringObject> NMF::Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    try
    {
        if (params.size() > 0)
            ParseParameters(params);

        clock_t begin = clock();

        CheckNonNegativity();

        PreProcessing();

        InitializateFirstValues();

        double divergence = 0;

        double lastDivergance = 0;

        int numOfSteps = 0;

        WBiclusters = arma::zeros<arma::umat>(p, expectedBiClusterCount);
        HBiclusters = arma::zeros<arma::umat>(n, expectedBiClusterCount);


        //////////////////////////////////////////////////////////////////////////////////////////////////

        for(uint i = 0; i < dataMatrix->data.n_rows; ++i)
        {
            if (i < 505)
                dataMatrix->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Prostate");
            else if (i > 1008)
                dataMatrix->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Head and Neck");
            else
                dataMatrix->classLabels.emplace_back(-1, 82, Enums::LabelType::RowClassLabel, i, "Thyroid");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////

        if (SupervisedDim >= 0)
        {
            QList<QString> distinstClass;
            QList<Label> classesValues;

            for(uint i = 0; i < dataMatrix->classLabels.size(); ++i)
            {
                if (SupervisedDim == 0 && dataMatrix->classLabels[i].idLabelType == Enums::LabelType::RowClassLabel)
                {
                    classesValues.append(dataMatrix->classLabels[i]);
                    if (!distinstClass.contains(dataMatrix->classLabels[i].value))
                    {
                        distinstClass.append(dataMatrix->classLabels[i].value);
                    }
                }
                else if (SupervisedDim == 1 && dataMatrix->classLabels[i].idLabelType == Enums::LabelType::ColumnClassLabel)
                {
                    classesValues.append(dataMatrix->classLabels[i]);
                    if (!distinstClass.contains(dataMatrix->classLabels[i].value))
                    {
                        distinstClass.append(dataMatrix->classLabels[i].value);
                    }
                }
            }

            if (SupervisedDim == 0)
            {
                arma::umat CurrWBiClusters;

                for (int i = 0; i < distinstClass.size(); ++i)
                {
                    arma::uvec Wvec = arma::sort_index(WMatrix.col(i));

                    for(int r = 0; r < p; ++r)
                    {
                        if (distinstClass[i] == classesValues[r].value)
                        {
                            Wvec(r) = 2.0;
                        }
                        else
                        {
                            Wvec(r) = 1.0;
                        }
                    }


                    CurrWBiClusters = arma::join_rows(CurrWBiClusters, Wvec);
                }

                WMatrix = WMatrix % CurrWBiClusters;

                // Normalization step
                WMatrix = WMatrix % arma::repmat(1./arma::sum(WMatrix,0), p, 1);
            }
        }

        while (numOfSteps++ < maxNumberOfSteps)
        {
            UpdateStep();

            divergence = DivernegceValue();

            qDebug() << numOfSteps << ": " << divergence;

            features.emplace_back(Enums::Divergence, divergence, numOfSteps);

            //Logger.Log(String.Format("Method: {0}, Step: {1}, Divergence Value: {2}. Count {3}", (Methods)Method, NumOfSteps, divergence, count), LogTypes.Info);

            if (std::abs(divergence - lastDivergance) < 10e-16)
            {
                break;
            }
            else
            {
                lastDivergance = divergence;
            }

            if (CheckBiClusters()) count++; else count = 0;

            if (count >= conectivityStepNumber)
            {
                break;
            }

            if (useConnectivity)
            {
                if (ConnectivityMatrix()) count++; else count = 0;

                if (count >= conectivityStepNumber)
                {
                    break;
                }
            }
        }

        //if (SendInfo != null)
        //    SendInfo(this.MaxNumberOfSteps - NumOfSteps);

        divergence = DivernegceValue();

        //Logger.Log("Done calculations", LogTypes.Info);

        foundedBiclusters = GetBiclusters();

        if (foundedBiclusters.size() == 0)
        {
            qDebug() << "Panic! Panic! Panic!";
        }

        PostProcessingTask();

        //std::shared_ptr<BiclusteringObject> retVal;

        //retVal;

        clock_t end = clock();

        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        qDebug() << "Time spend: " << time_spent;

        return shared_from_this();
        //return std::shared_ptr<BiclusteringObject>(this);
    }
    catch (...)
    {
        //Logger.Log(String.Format("Error in Compute. Message: {0}, Stack Trace: {1}", ex.Message, ex.StackTrace), LogTypes.Error);

        std::shared_ptr<BiclusteringObject> null;

        return null;
    }
}

void NMF::RebuildBiclusters()
{
    foundedBiclusters = GetBiclusters();

    if (foundedBiclusters.size() == 0)
    {
        qDebug() << "Panic! Panic! Panic!";
    }
}

std::vector<int> NMF::GetWBicluster(int k, Enums::ExtractingMethod extractingType, double threshold, uint maxBiclusterSize)
{
    std::vector<int> retVal;

    std::vector<std::tuple<int, double>> indexes;

    double max = arma::max(WMatrix.col(k));//.max();

    if (threshold > -1)
    {
        double average = arma::mean(WMatrix.col(k));
        double mse = arma::accu(arma::square(WMatrix.col(k) - average));
        double StDev = std::sqrt(mse / n);

        switch (extractingType)
        {
            case Enums::Zero:
                threshold = 0;
                break;
            case Enums::STD_1:
                if (average - StDev > 0)
                {
                    threshold = average - StDev;
                }
                break;
            case Enums::STD_2:
                if (average - (2 * StDev) > 0)
                {
                    threshold = average - (2 * StDev);
                }
                break;
            case Enums::Average:
                if (average > 0)
                {
                    threshold = average;
                }
                break;
            case Enums::Avg_plus_1_STD:
                if (average + StDev > 0)
                {
                    threshold = average + StDev;
                }
                break;
            case Enums::Avg_plus_2_STD:
                if (average + (2 * StDev) > 0)
                {
                    threshold = average + (2 * StDev);
                }
                break;
            case Enums::Avg_plus_3_STD:
                if (average + (3 * StDev) > 0)
                {
                    threshold = average + (3 * StDev);
                }
                break;
            case Enums::Quadrille:
                threshold = max * 0.75;
                break;
            default:
                threshold = 0;
                break;
        }
    }
    else
    {
        threshold = max * threshold;
    }

    for (int i = 0; i < p; ++i)
    {
        double value = std::abs(WMatrix(i, k));

        if (value >= threshold)
        {
            indexes.push_back(std::tuple<int, double>(i, value));
        }
    }

    std::sort(indexes.begin(), indexes.end(), [](const std::tuple<int, double> &m1, const std::tuple<int, double> &m2) -> bool { return std::get<1>(m1) > std::get<1>(m2); });

    for (uint i = 0; i < indexes.size() && i < maxBiclusterSize; ++i)
    {
        retVal.push_back(std::get<0>(indexes[i]));
    }

    return retVal;
}

std::vector<int> NMF::GetHBicluster(int k, Enums::ExtractingMethod extractingType, double threshold, uint maxBiclusterSize)
{

    std::vector<int> retVal;

    std::vector<std::tuple<int, double>> indexes;

    double max = arma::max(HMatrix.row(k));//.max();

    if (threshold > -1)
    {
        double average = arma::mean(HMatrix.row(k));
        double mse = arma::accu(arma::square(HMatrix.row(k) - average));
        double StDev = std::sqrt(mse / n);

        switch (extractingType)
        {
            case Enums::Zero:
                threshold = 0;
                break;
            case Enums::STD_1:
                if (average - StDev > 0)
                {
                    threshold = average - StDev;
                }
                break;
            case Enums::STD_2:
                if (average - (2 * StDev) > 0)
                {
                    threshold = average - (2 * StDev);
                }
                break;
            case Enums::Average:
                if (average > 0)
                {
                    threshold = average;
                }
                break;
            case Enums::Avg_plus_1_STD:
                if (average + StDev > 0)
                {
                    threshold = average + StDev;
                }
                break;
            case Enums::Avg_plus_2_STD:
                if (average + (2 * StDev) > 0)
                {
                    threshold = average + (2 * StDev);
                }
                break;
            case Enums::Avg_plus_3_STD:
                if (average + (3 * StDev) > 0)
                {
                    threshold = average + (3 * StDev);
                }
                break;
            case Enums::Quadrille:
                threshold = max * 0.75;
                break;
            default:
                threshold = 0;
                break;
        }
    }
    else
    {
        threshold = max * threshold;
    }

    for (int i = 0; i < n; ++i)
    {
        double value = std::abs(HMatrix(k, i));

        if (value >= threshold)
        {
            indexes.push_back(std::tuple<int, double>(i, value));
        }
    }

    std::sort(indexes.begin(), indexes.end(), [](const std::tuple<int, double> &m1, const std::tuple<int, double> &m2) -> bool { return std::get<1>(m1) > std::get<1>(m2); });

    for (uint i = 0; i < indexes.size() && i < maxBiclusterSize; ++i)
    {
        retVal.push_back(std::get<0>(indexes[i]));
    }

    return retVal;
}

void NMF::TrimCluster1(std::shared_ptr<Bicluster>& bic, int bicNumber)
{
    bool NotDone = true;

    std::vector<int> clust2 = bic->cluster2;
    std::vector<int> clust1 = bic->cluster1;

    while (clust1.size() > 10 && NotDone)
    {
        //clock_t begin = clock();

        clust1 = GetWBicluster(bicNumber, exMethod, 0, clust1.size() - 1);

        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(-1, clust1, clust2, dataMatrix->AverageCorrelationValue(clust1, clust2), nullptr);

        //clock_t end = clock();

        //time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        //qDebug() << "Get cluster1 of " << bicNumber << " New ACV: " << *NewBic->ACV << " Size: (" << clust1.size() << ", " << clust2.size() << ") Time Spent: " << time_spent;

        if (*NewBic->ACV > *bic->ACV)
        {
            bic = NewBic;
        }
        else
        {
            NotDone = false;
        }
    }
}

void NMF::TrimCluster2(std::shared_ptr<Bicluster>& bic, int bicNumber)
{
    bool NotDone = true;

    std::vector<int> clust2 = bic->cluster2;
    std::vector<int> clust1 = bic->cluster1;

    while (clust2.size() > 10 && NotDone)
    {
        //clock_t begin = clock();

        clust2 = GetHBicluster(bicNumber, exMethod, 0, clust2.size() - 1);

        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(-1, clust1, clust2, dataMatrix->AverageCorrelationValue(clust1, clust2), nullptr);

        //clock_t end = clock();

        //time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        //qDebug() << "Get cluster2 of " << bicNumber << " New ACV: " << *NewBic->ACV << " Size: (" << clust1.size() << ", " << clust2.size() << ") Time Spent: " << time_spent;

        if (*NewBic->ACV > *bic->ACV)
        {
            bic = NewBic;
        }
        else
        {
            NotDone = false;
        }
    }
}


std::vector<std::shared_ptr<Bicluster>> NMF::GetBiclusters()
{
    std::vector<std::shared_ptr<Bicluster>> retVal;

    for (int i = 0; i < expectedBiClusterCount; ++i)
    {
        std::vector<int> clust2 = GetHBicluster(i, exMethod, 0, 100);  //n
        std::vector<int> clust1 = GetWBicluster(i, exMethod, 0, 100);  //p

        if (clust1.size() == 0 || clust2.size() == 0)
            return std::vector<std::shared_ptr<Bicluster>>();

        std::shared_ptr<Bicluster> bic = std::make_shared<Bicluster>(-1, clust1, clust2, dataMatrix->AverageCorrelationValue(clust1, clust2), nullptr);

        //qDebug() << "Get bicluster " << i << " Initial ACV: " << *bic->ACV << " Size: (" << clust1.size() << ", " << clust2.size() << ")";

        if (*bic->ACV < 1.0 && trimByACV)
        {
            if (clust1.size() > clust2.size())
            {
                TrimCluster1(bic, i);

                TrimCluster2(bic, i);
            }
            else
            {
                TrimCluster2(bic, i);

                TrimCluster1(bic, i);
            }
        }

        qDebug() << "Done bicluster " << i << ". Size <" << clust1.size() << ", " << clust2.size() << ">. ACV: " << *bic->ACV;

        retVal.push_back(bic);
    }

    return retVal;
}

bool NMF::CheckBiClusters()
{
    bool RetVal = true;

    arma::umat CurrHBiClusters;
    arma::umat CurrWBiClusters;

    for(int i = 0; i < expectedBiClusterCount; ++i)
    {
        arma::uvec Wvec = arma::sort_index(WMatrix.col(i));
        arma::uvec Hvec = arma::sort_index(HMatrix.row(i).t());

        CurrWBiClusters = arma::join_rows(CurrWBiClusters, Wvec);
        CurrHBiClusters = arma::join_rows(CurrHBiClusters, Hvec);
    }

    arma::umat equalW = CurrWBiClusters == WBiclusters;
    arma::umat equalH = CurrHBiClusters == HBiclusters;

    int Wtest = arma::accu(equalW);
    int Htest = arma::accu(equalH);

    if (Wtest == (p*expectedBiClusterCount) && Htest == (n*expectedBiClusterCount))
    {
        RetVal = true;
    }
    else
    {
        HBiclusters = CurrHBiClusters;
        WBiclusters = CurrWBiClusters;
        RetVal = false;
    }

    return RetVal;
}

int NMF::maxRowH(int c)
{
    int maxi;
    double v;
    v = 0;
    maxi = 0;
    for (int i = 0; i < expectedBiClusterCount; ++i)
    {
        if (HMatrix(i, c) > v)
        {
            v = HMatrix(i, c);
            maxi = i;
        }

    }
    return maxi;
}

bool NMF::ConnectivityMatrix()
{
    uint new_cc[n][n];

    for (int i = 0; i < n; ++i)
    {
        int maxi = maxRowH(i);
        for (int j = i; j < n; ++j)
        {
            if (i == j)
                new_cc[i][j] = 1;
            else
                if (maxi == maxRowH(j))
                    new_cc[i][j] = 1;
                else
                    new_cc[i][j] = 0;
        }
    }

    // CHECK IF IT IS IDENTICAL TO THE PREVIOUS ONE, IF SO, FINISH
    bool testb = true;

    for (int i = 0; i < n; ++i)
    {
        for (int j = i; j < n; ++j)
        {
            if (new_cc[i][j] != old_cc(i,j))
            {
                testb = false;
                break;
            }
        }
        if (!testb) break;
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = i; j < n; ++j)
            old_cc(i,j) = new_cc[i][j];
    }

    return testb;
}
