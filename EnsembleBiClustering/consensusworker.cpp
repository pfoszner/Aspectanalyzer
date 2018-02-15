#include "consensusworker.h"

void ConsensusWorker::run()
{
    if (dataMatrix != nullptr)
    {
        for( std::shared_ptr<Bicluster>& item : connected)
        {
            if (item->cluster1.size() > 0 && item->cluster2.size() > 0)
                item->SetFeature(choosenMeasure, dataMatrix->CalculateQualityMeasure(choosenMeasure, item->cluster1, item->cluster2));
            else
                item->SetFeature(choosenMeasure, 0.0);
        }
    }

    std::vector<ClusterItem> Cluster1;
    std::vector<ClusterItem> Cluster2;

    for ( std::shared_ptr<Bicluster>& item : connected)
    {
        for (int c1 : item->cluster1)
        {
            auto it = std::find_if(Cluster1.begin(), Cluster1.end(), [c1](ClusterItem iter) { return iter.Index == c1; } );

            if (it != Cluster1.end())
                it->IncreaseWeight(*item->GetFeature(choosenMeasure));
            else
                Cluster1.emplace_back(c1, *item->GetFeature(choosenMeasure));
        }

        for (int c2 : item->cluster2)
        {
            auto it = std::find_if(Cluster2.begin(), Cluster2.end(), [c2](ClusterItem iter) { return iter.Index == c2; } );

            if (it != Cluster2.end())
                it->IncreaseWeight(*item->GetFeature(choosenMeasure));
            else
                Cluster2.emplace_back(c2, *item->GetFeature(choosenMeasure));
        }
    }

    if (extractType == MergeType::Standard)
    {
        double MinWeight = orgResultsSize;

        std::vector<int> FinalCluster1;
        std::vector<int> FinalCluster2;

        bool done = false;

        while (!done)
        {
            FinalCluster1.clear();

            for (ClusterItem item : Cluster1)
            {
                if (item.Weight() >= MinWeight)
                {
                    FinalCluster1.push_back(item.Index);
                }
            }

            if ((FinalCluster1.size() >= minClusterSize) || (MinWeight <= 0.1))
                done = true;
            else
                MinWeight *= 0.9;
        }

        MinWeight = (int)std::round(orgResultsSize * percentage);

        done = false;

        while (!done)
        {
            FinalCluster2.clear();

            for (ClusterItem item : Cluster2)
            {
                if (item.Weight() >= MinWeight)
                {
                    FinalCluster2.push_back(item.Index);
                }
            }

            if ((FinalCluster2.size() >= minClusterSize) || (MinWeight <= 0.1))
                done = true;
            else
                MinWeight *= 0.9;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2);



    }
    else if (extractType == MergeType::ByACV)
    {
        std::vector<int> FinalCluster1(Cluster1.size());
        std::vector<int> FinalCluster2(Cluster2.size());

        //TOPTip: GPervTools & TCMalloc

        std::transform(Cluster1.begin(), Cluster1.end(), FinalCluster1.begin(), [](const ClusterItem& c){ return c.Index; });
        std::transform(Cluster2.begin(), Cluster2.end(), FinalCluster2.begin(), [](const ClusterItem& c){ return c.Index; });

        bool done = false;

        Bicluster Final(-1, FinalCluster1, FinalCluster2);

        Final.SetFeature(choosenMeasure, dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2));

        double currentValue = *Final.GetFeature(choosenMeasure);
        double candidateValue = 0.0;

        while (!done)
        {
            if (FinalCluster1.size() <= minClusterSize)
            {
                Cluster1.clear();
            }

            if (FinalCluster2.size() <= minClusterSize)
            {
                Cluster2.clear();
            }

            if (Cluster1.size() > 0)
            {
                auto iter = std::min_element(Cluster1.begin(), Cluster1.end(), [](const ClusterItem& c1, const ClusterItem& c2){ return c1.Index < c2.Index; });
                int Index = iter->Index;

                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [Index](int r){ return r == Index; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                auto criter = std::remove_if(Cluster1.begin(), Cluster1.end(), [Index](ClusterItem& r) { return r.Index == Index; });
                Cluster1.erase(criter, Cluster1.end());

                candidateValue = dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2);

                //qDebug() << "ByACV: Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size();

                if (candidateValue <= currentValue)
                {
                    FinalCluster1.push_back(Index);
                }
                else
                {
                    currentValue = candidateValue;
                }
            }

            if (Cluster2.size() > 0)
            {
                auto iter = std::min_element(Cluster2.begin(), Cluster2.end(), [](const ClusterItem& c1, const ClusterItem& c2){ return c1.Index < c2.Index; });
                int Index = iter->Index;

                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [Index](int r){ return r == Index; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                auto criter = std::remove_if(Cluster2.begin(), Cluster2.end(), [Index](const ClusterItem& r) { return r.Index == Index; });
                Cluster2.erase(criter, Cluster2.end());

                candidateValue = dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2);

                //qDebug() << "ByACV: (" << candidateValue << ") Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size() << "Final Size 1: " << FinalCluster1.size() << " Final Size 2: " << FinalCluster2.size();

                if (candidateValue <= currentValue)
                {
                    FinalCluster2.push_back(Index);
                }
                else
                {
                    currentValue = candidateValue;
                }
            }

            if (Cluster1.size() == 0 && Cluster2.size() == 0)
                done = true;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2);

    }
    else if (extractType == MergeType::ByACVHeuristic)
    {
        std::vector<int> FinalCluster1(Cluster1.size());
        std::vector<int> FinalCluster2(Cluster2.size());

        //TOPTip: GPervTools & TCMalloc

        std::transform(Cluster1.begin(), Cluster1.end(), FinalCluster1.begin(), [](const ClusterItem& c){ return c.Index; });
        std::transform(Cluster2.begin(), Cluster2.end(), FinalCluster2.begin(), [](const ClusterItem& c){ return c.Index; });

        bool done = false;

        while (!done)
        {
            double min1 = std::numeric_limits<double>::max();
            double min2 = std::numeric_limits<double>::max();

            int index1 = -1;
            int index2 = -1;

            if (FinalCluster1.size() <= minClusterSize)
            {
                Cluster1.clear();
            }

            if (FinalCluster2.size() <= minClusterSize)
            {
                Cluster2.clear();
            }

            double currentValue = dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2);

            if (Cluster1.size() > 0)
            {
                auto iter = std::min_element(Cluster1.begin(), Cluster1.end(), [](ClusterItem& c1, ClusterItem& c2){ return c1.Weight() < c2.Weight(); });

                index1 = iter->Index;

                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [index1](const int r) { return r == index1; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                double Candidate = dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2);

                min1 = Candidate - currentValue;

                FinalCluster1.push_back(index1);
            }

            if (Cluster2.size() > 0)
            {
                auto iter = std::min_element(Cluster2.begin(), Cluster2.end(), [](const ClusterItem& c1,  const ClusterItem& c2){ return c1.Weight() < c2.Weight(); });

                index2 = iter->Index;

                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [index2](int r) { return r == index2; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                double Candidate = dataMatrix->CalculateQualityMeasure(choosenMeasure, FinalCluster1, FinalCluster2);

                min2 = Candidate - currentValue;

                FinalCluster2.push_back(index2);
            }

            if ((index1 > -1) && ((min1 > 0 && min1 >= min2) || (min1 > 0 && min2 == std::numeric_limits<double>::max())))
            {
                auto riter = std::remove_if(FinalCluster1.begin(), FinalCluster1.end(), [index1](int r) { return r == index1; });
                FinalCluster1.erase(riter, FinalCluster1.end());

                auto criter = std::remove_if(Cluster1.begin(), Cluster1.end(), [index1](const ClusterItem& r) { return r.Index == index1; });
                Cluster1.erase(criter, Cluster1.end());
            }
            else if (min1 <= 0 && min1 < std::numeric_limits<double>::max())
            {
                auto riter = std::remove_if(Cluster1.begin(), Cluster1.end(), [index1](const ClusterItem& r) { return r.Index == index1; });
                Cluster1.erase(riter, Cluster1.end());
            }

            if ((index2 > -1) && ((min2 > 0 && min2 >= min1) || (min2 > 0 && min1 == std::numeric_limits<double>::max())))
            {
                auto riter = std::remove_if(FinalCluster2.begin(), FinalCluster2.end(), [index2](int r) { return r == index2; });
                FinalCluster2.erase(riter, FinalCluster2.end());

                auto criter = std::remove_if(Cluster2.begin(), Cluster2.end(), [index2](const ClusterItem& r) { return r.Index == index2; });
                Cluster2.erase(criter, Cluster2.end());
            }
            else if (min2 <= 0 && min2 < std::numeric_limits<double>::max())
            {
                auto riter = std::remove_if(Cluster2.begin(), Cluster2.end(), [index2](const ClusterItem& r) { return r.Index == index2; });
                Cluster2.erase(riter, Cluster2.end());
            }

            //qDebug() << "ByACVHeuristic (" << currentValue <<  "): Size 1: " << Cluster1.size() << " Size 2: " << Cluster2.size() << "Final Size 1: " << FinalCluster1.size() << " Final Size 2: " << FinalCluster2.size();

            if (Cluster1.size() == 0 && Cluster2.size() == 0)
                done = true;
        }

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2);

    }
    else if (extractType == MergeType::None)
    {
        std::vector<int> FinalCluster1(Cluster1.size());
        std::vector<int> FinalCluster2(Cluster2.size());

        std::transform(Cluster1.begin(), Cluster1.end(), FinalCluster1.begin(), [](const ClusterItem& c){ return c.Index; });
        std::transform(Cluster2.begin(), Cluster2.end(), FinalCluster2.begin(), [](const ClusterItem& c){ return c.Index; });

        bic = std::make_shared<Bicluster>(-1, FinalCluster1, FinalCluster2);
    }
    else if (extractType == MergeType::GainFunction)
    {
        //Build loss matrix

            arma::mat lossMatrix = arma::zeros(dataMatrix->data.n_rows, dataMatrix->data.n_cols);

            long area = dataMatrix->data.n_rows * dataMatrix->data.n_cols;

            long sumInside = 0;
            long penaltyForZero = penaltyForGainFunction;
            long zerosInside = area;
            long sumOutside = 0;

            for ( std::shared_ptr<Bicluster>& item : connected)
            {
                for (int c1 : item->cluster1)
                {
                    for (int c2 : item->cluster2)
                    {
                        if (lossMatrix(c1, c2) == 0)
                            zerosInside--;

                        lossMatrix(c1, c2) += 1;
                        sumInside++;
                        //if (penaltyForZero < lossMatrix(c1, c2))
                        //    penaltyForZero = lossMatrix(c1, c2);
                    }
                }
            }

            //if (zerosInside < area)
                //penaltyForZero = sumInside / (area - zerosInside);

            //lossMatrix.save("lossMatrix.mat", arma::raw_ascii);
            //                    lossMatrix.save("lossMatrix_bin.mat", arma::raw_binary);
            //                    lossMatrix.save("lossMatrix_csv.mat", arma::csv_ascii);

            std::vector<int> F;

            for (uint i = 0; i < dataMatrix->data.n_rows; ++i)
            {
                F.push_back(i);
            }

            std::vector<int> E;
            for (uint i = 0; i < dataMatrix->data.n_cols; ++i)
            {
                E.push_back(i);
            }
/*
            std::vector<int> removeF;

            for(int f : F)
            {
                bool remove = true;

                for(int e : E)
                {
                    if (lossMatrix(f,e) > 0)
                    {
                        remove = false;
                    }
                }

                if (remove)
                {
                    removeF.push_back(f);
                }
            }

            for(int f = (int)removeF.size()-1; f >= 0; --f)
            {
                F.erase(F.begin()+removeF[f]);
                lossMatrix.shed_row(f);
            }

            //////////////////////////////////////////////////////////////

            std::vector<int> removeE;

            for(int e : E)
            {
                bool remove = true;

                for(int f = 1; f < F.size(); ++f)
                {
                    if (lossMatrix(f,e) > 0)
                    {
                        remove = false;
                    }
                }

                if (remove)
                {
                    removeE.push_back(e);
                }
            }

            for(int e = (int)removeE.size()-1; e >= 0; --e)
            {
                E.erase(E.begin()+removeE[e]);
                lossMatrix.shed_col(e);
            }
*/
            double gainSub = sumInside - (zerosInside * penaltyForZero) - sumOutside;

            bool done = false;

            do
            {
                gainSub = sumInside - (zerosInside * penaltyForZero) - sumOutside;

                double gainF = -1;
                int Fstar = -1;
                bool Fcalc = false;

                if (F.size() > 2)
                {
                    for(uint f = 0; f < F.size(); ++f)
                    {
                        arma::rowvec rowF = lossMatrix.row(f);

                        long sumOut = arma::accu(rowF);
                        long zerosOut = 0;

                        for(auto elem : rowF)
                        {
                            if (elem == 0)
                                zerosOut++;;
                        }

                        double tmpSumInside = sumInside - sumOut;
                        double tmpZerosInside = zerosInside - zerosOut;
                        double tmpSumOutside = sumOutside + sumOut;

                        //sumInside - (zerosInside * penaltyForZero) - sumOutside
                        //double test = ((sumInside - sumOut) - ((zerosInside - zerosOut) * penaltyForZero) - sumOutside);// / ((lossMatrix.n_rows) * lossMatrix.n_cols);

                        double tmpGainF = tmpSumInside - (tmpZerosInside * penaltyForZero) - tmpSumOutside;

                        if (!Fcalc || tmpGainF > gainF)
                        {
                            gainF = tmpGainF;
                            Fstar = f;
                            Fcalc = true;
                        }
                    }
                }

                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                double gainE = -1;
                int Estar = -1;
                bool Ecalc = false;

                if (E.size() > 2)
                {
                    for(uint e = 0; e < E.size(); ++e)
                    {
                        arma::colvec colE = lossMatrix.col(e);

                        long sumOut = arma::accu(colE);
                        long zerosOut = 0;

                        for(auto elem : colE)
                        {
                            if (elem == 0)
                                zerosOut++;;
                        }

                        double tmpSumInside = sumInside - sumOut;
                        double tmpZerosInside = zerosInside - zerosOut;
                        double tmpSumOutside = sumOutside + sumOut;

                        //double test = ((sumInside - sumOut) - ((zerosInside - zerosOut) * penaltyForZero) - sumOutside);// / ((lossMatrix.n_rows) * lossMatrix.n_cols);;

                        double tmpGainE = tmpSumInside - (tmpZerosInside * penaltyForZero) - tmpSumOutside;


                        if (!Ecalc || tmpGainE > gainE)
                        {
                            gainE = tmpGainE;
                            Estar = e;
                            Ecalc = true;
                        }
                    }
                }

                double maxGain = gainSub;// / (lossMatrix.n_rows * lossMatrix.n_cols);
                if (Ecalc && maxGain < gainE) maxGain = gainE;
                if (Fcalc && maxGain < gainF) maxGain = gainF;

                if (maxGain > gainSub)// / (lossMatrix.n_rows * lossMatrix.n_cols))
                {
                    gainSub = maxGain;

                    if (gainSub == gainF)
                    {
                        arma::rowvec rowF = lossMatrix.row(Fstar);

                        long sumOut = arma::accu(rowF);
                        long zerosOut = 0;

                        for(auto elem : rowF)
                        {
                            if (elem == 0)
                                zerosOut++;;
                        }

                        F.erase(F.begin() + Fstar);

                        sumInside -= sumOut;
                        zerosInside -= zerosOut;
                        sumOutside += sumOut;

                        lossMatrix.shed_row(Fstar);
                    }
                    if (gainSub == gainE)
                    {
                        arma::colvec colE = lossMatrix.col(Estar);

                        long sumOut = arma::accu(colE);
                        long zerosOut = 0;

                        for(auto elem : colE)
                        {
                            if (elem == 0)
                                zerosOut++;;
                        }

                        E.erase(E.begin() + Estar);

                        sumInside -= sumOut;
                        zerosInside -= zerosOut;
                        sumOutside += sumOut;

                        lossMatrix.shed_col(Estar);
                    }

                    //qDebug() << "New Min value " << maxGain << ", F: " << F.size() << ", E: " << E.size();
                }
                else
                {
                    done = true;
                }
            }
            while (!done);

            //Return k-th tricluster

            if (F.size() > 1 && E.size() > 1)
            {
                bic = std::make_shared<Bicluster>(-1, F, E);
            }

    }
}
