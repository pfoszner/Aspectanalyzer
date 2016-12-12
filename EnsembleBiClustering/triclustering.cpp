#include "triclustering.h"

std::shared_ptr<BiclusteringObject> TriClustering::Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params)
{
    //TODO: params
    params.clear();

    try
    {
        std::vector<std::shared_ptr<Bicluster>> TriClusters;

        TriClusters = DoTheTriClustering(GetBiclustersList());

        for(auto item : TriClusters)
        {
            foundedBiclusters.emplace_back(item);
        };

        PostProcessingTask();

        return shared_from_this();
    }
    catch (...)
    {
        //Logger.Log(String.Format("Error in Compute. Message: {0}, Stack Trace: {1}", ex.Message, ex.StackTrace), LogTypes.Error);

        std::shared_ptr<BiclusteringObject> null;

        return null;
    }
}

std::vector<std::shared_ptr<Bicluster>> TriClustering::DoTheTriClustering(std::vector<std::shared_ptr<Bicluster>> Biclusters)
{
    std::vector<std::shared_ptr<Bicluster>> RetVal;

    try
    {
        BuildBinaryCube(Biclusters, dataMatrix->data.n_rows, dataMatrix->data.n_cols);

        int k = 0;

        do
        {
            k++;
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

            std::vector<int> B;
            for (uint i = 0; i < Biclusters.size(); ++i)
            {
                B.push_back(i);
            }

            //////////////////////////////////////////////////////////////

            std::vector<int> removeB;

            for(int b : B)
            {
                bool remove = true;

                for(int f : F)
                {
                    for(int e : E)
                    {
                        if (cube(f,e,b) == 1)
                        {
                            remove = false;
                        }
                    }
                }

                if (remove)
                {
                    removeB.push_back(b);
                }
            }

            for(int b = removeB.size()-1; b > 0; --b)
            {
                B.erase(B.begin()+removeB[b]);
            }

            //////////////////////////////////////////////////////////////

            std::vector<int> removeF;

            for(int f : F)
            {
                bool remove = true;

                for(int e : E)
                {
                    for(int b : B)
                    {
                        if (cube(f,e,b) == 1)
                        {
                            remove = false;
                        }
                    }
                }

                if (remove)
                {
                    removeF.push_back(f);
                }
            }

            for(int f = removeF.size()-1; f > 0; --f)
            {
                F.erase(F.begin()+removeF[f]);
            }

            //////////////////////////////////////////////////////////////

            std::vector<int> removeE;

            for(int e : E)
            {
                bool remove = true;

                for(int f : F)
                {
                    for(int b : B)
                    {
                        if (cube(f,e,b) == 1)
                        {
                            remove = false;
                        }
                    }
                }

                if (remove)
                {
                    removeE.push_back(e);
                }
            }

            for(int e = removeE.size()-1; e > 0; --e)
            {
                E.erase(E.begin()+removeE[e]);
            }

            if (F.size() <= 1 || E.size() <= 1 || B.size() <= 1)
                break;

            double lossSub = F.size() * E.size() * Biclusters.size();

            bool done = false;

            do
            {
                std::vector<double> FLoss(F.size());

                for(uint f = 0; f < F.size(); ++f)
                {
                    std::vector<int> tF = F;

                    tF.erase(tF.begin() + f);

                    SingleLossWorker *st = new SingleLossWorker(FLoss.begin() + f, tF, E, B, &cube);

                    QThreadPool::globalInstance()->start(st);
                }

                QThreadPool::globalInstance()->waitForDone();

                auto resultF = std::min_element(FLoss.begin(), FLoss.end());

                //int Fstar = F[std::distance(FLoss.begin(), resultF)];

                double LossF = *resultF;

                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                std::vector<double> ELoss(E.size());

                for(uint e = 0; e < E.size(); ++e)
                {
                    std::vector<int> tE = E;

                    tE.erase(tE.begin() + e);

                    SingleLossWorker *st = new SingleLossWorker(std::next(ELoss.begin(), e), F, tE, B, &cube);

                    QThreadPool::globalInstance()->start(st);
                }

                QThreadPool::globalInstance()->waitForDone();

                auto resultE = std::min_element(ELoss.begin(), ELoss.end());

                double LossE = *resultE;

                //int Estar = E[std::distance(ELoss.begin(), resultE)];

                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                std::vector<double> BLoss(B.size());

                for(uint b = 0; b < B.size(); ++b)
                {
                    std::vector<int> tB = B;

                    tB.erase(tB.begin() + b);

                    SingleLossWorker *st = new SingleLossWorker(BLoss.begin() + b, F, E, tB, &cube);

                    QThreadPool::globalInstance()->start(st);
                }

                QThreadPool::globalInstance()->waitForDone();

                auto resultB = std::min_element(BLoss.begin(), BLoss.end());

                double LossB = *resultB;

                //int Bstar = B[std::distance(BLoss.begin(), resultB)];

                double minLoss = LossE;
                if (minLoss > LossF) minLoss = LossF;
                if (minLoss > LossB) minLoss = LossB;

                if (minLoss < lossSub && minLoss > 0)
                {
                    lossSub = minLoss;

                    if (lossSub == LossF)
                    {
                        F.erase(F.begin() + std::distance(FLoss.begin(), resultF));
                    }
                    if (lossSub == LossE)
                    {
                        E.erase(E.begin() + std::distance(ELoss.begin(), resultE));
                    }
                    if (lossSub == LossB)
                    {
                        B.erase(B.begin() + std::distance(BLoss.begin(), resultB));
                    }

                    qDebug() << "New Min value " << minLoss << ", F: " << F.size() << ", E: " << E.size() << ", B: " << B.size();

                    if (B.size() == 0)
                    {
                        done = true;
                        qDebug() << "Done k = " << k << ", F: " << F.size() << ", E: " << E.size() << ", B: " << B.size();
                    }
                }
                else
                {
                    done = true;
                    qDebug() << "Done k = " << k << ", F: " << F.size() << ", E: " << E.size() << ", B: " << B.size();
                }
            }
            while (!done);

            //Return k-th tricluster

            std::shared_ptr<Bicluster> New = std::make_shared<Bicluster>(-1, F, E, dataMatrix->AverageCorrelationValue(F, E), nullptr);

            RetVal.push_back(New);

            for (int b : B)
            {
                //Clear bi-clusters that were included in current
                for (uint f = 0; f < cube.n_rows; ++f)
                {
                    for (uint e = 0; e < cube.n_cols; ++e)
                    {
                        cube(f,e,b) = (short)0;
                    }
                }
            }
        }
        while (k < expectedBiClusterCount);
    }
    catch (...)
    {
        qDebug() << "Exception";
        //Logger.Log(String.Format("Error Message: {0}, StackTrace: {1}", ex.Message, ex.StackTrace), Common.LogTypes.Error);
    }

    return RetVal;
}

void TriClustering::BuildBinaryCube(const std::vector<std::shared_ptr<Bicluster>>& Biclusters, int dim1, int dim2)
{
    //cube = arma::Cube<short>(dim1, dim2, Biclusters.size());

    cube.zeros(dim1, dim2, Biclusters.size());

    //double Surface = dim1 * dim2 * Biclusters.size();

    //int NumOfPoints = (int)Math.Round(Surface * p);

    try
    {
        for (uint b = 0; b < Biclusters.size(); ++b)
        {
            for (int i : Biclusters[b]->cluster1)
            {
                for (int j : Biclusters[b]->cluster2)
                {
                    cube(i, j, b) = (short)1;
                }
            }
        }

        //        #region Installing perturbation rate
        //        if (p > 0.0)
        //        {
        //            Random generator = new Random(DateTime.UtcNow.Millisecond);

        //            List<int> IndexesToSwitch = new List<int>();

        //            for (int i = 0; i < NumOfPoints; ++i)
        //            {
        //                IndexesToSwitch.Add(generator.Next((int)Surface));
        //            }

        //            int index = 0;

        //            for (int b = 0; b < Biclusters.Count; ++b)
        //            {
        //                for (int i = 0; i < dim1; ++i)
        //                {
        //                    for (int j = 0; j < dim2; ++j)
        //                    {
        //                        if (IndexesToSwitch.Contains(index))
        //                        {
        //                            if (RetVal[String.Format("{0};{1};{2}", i, j, b)] == 1)
        //                            {
        //                                RetVal[String.Format("{0};{1};{2}", i, j, b)] = 0;
        //                            }
        //                            else
        //                            {
        //                                RetVal[String.Format("{0};{1};{2}", i, j, b)] = 1;
        //                            }
        //                        }

        //                        index++;
        //                    }
        //                }
        //            }
        //        }
        //        #endregion
    }
    catch (...)
    {
        //Logger.Log(String.Format("Error Message: {0}, StackTrace: {1}", ex.Message, ex.StackTrace), Common.LogTypes.Error);
        qDebug() << "Panic Cube";
    }

}

void SingleLossWorker::run()
{
    double retVal = 0.0;

    int vecIndexB = 0;

    for (uint b = 0; b < (*cube).n_slices; ++b)
    {
        int vecIndexF = 0;

        for (uint f = 0; f < (*cube).n_rows; ++f)
        {
            int vecIndexE = 0;

            for (uint e = 0; e < (*cube).n_cols; ++e)
            {
                // Count zeros inside selected area
                if ((f == (uint)F[vecIndexF])&&(e == (uint)E[vecIndexE])&&(b == (uint)B[vecIndexB]))
                {
                    if ((*cube)(f,e,b) == 0)
                        retVal += 1.0;
                }
                // Count ones outside selected area
                else
                {
                    if ((*cube)(f,e,b) == 1)
                        retVal += 1.0;
                }

                if (e == (uint)E[vecIndexE])
                {
                    vecIndexE++;
                    if ((uint)vecIndexE >= E.size())
                    {
                        vecIndexE = 0;
                    }
                }
            }

            if (f == (uint)F[vecIndexF])
            {
                vecIndexF++;
                if ((uint)vecIndexF >= F.size())
                {
                    vecIndexF = 0;
                }
            }
        }

        if (b == (uint)B[vecIndexB])
        {
            vecIndexB++;
            if ((uint)vecIndexB >= B.size())
            {
                vecIndexB = 0;
            }
        }
    }

    *pointer = retVal / (double)(F.size() * E.size() * B.size());
}
