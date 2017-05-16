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
    catch (std::exception& e)
    {
        //Logger.Log(String.Format("Error in Compute. Message: {0}, Stack Trace: {1}", ex.Message, ex.StackTrace), LogTypes.Error);

        qDebug() << e.what();

        std::shared_ptr<BiclusteringObject> null;

        return null;
    }
}

std::vector<std::shared_ptr<Bicluster>> TriClustering::DoTheTriClustering(std::vector<std::shared_ptr<Bicluster>> Biclusters)
{
    std::vector<std::shared_ptr<Bicluster>> RetVal;

    try
    {
        int k = 0;

        do
        {
            if (Biclusters.size() == 0)
                break;

            BuildBinaryCube(Biclusters, dataMatrix->data.n_rows, dataMatrix->data.n_cols);

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

            if (removeF.size() > 0)
                RemoveCubeRow(removeF);

            for(int f = removeF.size()-1; f >= 0; --f)
            {
                F.erase(F.begin()+removeF[f]);
            }

            //////////////////////////////////////////////////////////////

            std::vector<int> removeE;

            for(int e : E)
            {
                bool remove = true;

                for(int f = 1; f < F.size(); ++f)
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

            if (removeE.size() > 0)
                RemoveCubeColumn(removeE);

            for(int e = removeE.size()-1; e >= 0; --e)
            {
                E.erase(E.begin()+removeE[e]);
            }

            if (F.size() <= 1 || E.size() <= 1 || B.size() <= 1)
                break;

            double lossSub = F.size() * E.size() * Biclusters.size();

            bool done = false;

            double onesOutside = 0;
            double zerosInside = cube.n_elem - arma::accu(cube);

            do
            {
                double LossF = -1;
                int Fstar = -1;

                if (F.size() > 1)
                {
                    for(uint f = 0; f < F.size(); ++f)
                    {
                        arma::ucube rowF = cube.tube(f,0,f,E.size()-1);

                        double onesOut = arma::accu(rowF);
                        double zerosOut = rowF.n_elem - onesOut;

                        double tmpLossF = (onesOutside + onesOut) + (zerosInside - zerosOut);

                        tmpLossF /= (double)((F.size()-1)*E.size()*B.size());

                        if (Fstar < 0 || tmpLossF < LossF)
                        {
                            LossF = tmpLossF;
                            Fstar = f;
                        }
                    }
                }

                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                double LossE = -1;
                int Estar = -1;

                if (E.size() > 1)
                {
                    for(uint e = 0; e < E.size(); ++e)
                    {
                        arma::ucube colE = cube.tube(0,e,F.size()-1,e);

                        double onesOut = arma::accu(colE);
                        double zerosOut = colE.n_elem - onesOut;

                        double tmpLossE = (onesOutside + onesOut) + (zerosInside - zerosOut);

                        tmpLossE /= (double)((E.size()-1)*F.size()*B.size());

                        if (Estar < 0 || tmpLossE < LossE)
                        {
                            LossE = tmpLossE;
                            Estar = e;
                        }
                    }
                }
                ///////////////////////////////////////////////////////////////////////////////////////////////////////

                double LossB = -1;
                int Bstar = -1;

                if (B.size() > 1)
                {
                    for(uint b = 0; b < B.size(); ++b)
                    {
                        auto bicB = cube.slice(b);

                        double onesOut = arma::accu(bicB);
                        double zerosOut = bicB.n_elem - onesOut;

                        double tmpLossB = (onesOutside + onesOut) + (zerosInside - zerosOut);

                        tmpLossB /= (double)((B.size()-1) * E.size() * F.size());

                        if (Bstar < 0 || tmpLossB < LossB)
                        {
                            LossB = tmpLossB;
                            Bstar = b;
                        }
                    }
                }

                double minLoss = 2;
                if (LossE > 0 && minLoss > LossE) minLoss = LossE;
                if (LossF > 0 && minLoss > LossF) minLoss = LossF;
                if (LossB > 0 && minLoss > LossB) minLoss = LossB;

                if (minLoss < lossSub && minLoss > 0 && minLoss < 2)
                {
                    lossSub = minLoss;

                    if (lossSub == LossF)
                    {
                        arma::ucube rowF = cube.tube(Fstar,0,Fstar,E.size()-1);

                        F.erase(F.begin() + Fstar);

                        double onesOut = arma::accu(rowF);
                        double zerosOut = rowF.n_elem - onesOut;

                        onesOutside += onesOut;
                        zerosInside -= zerosOut;

                        RemoveCubeRow(Fstar);
                    }
                    if (lossSub == LossE)
                    {
                        arma::ucube colE = cube.tube(0,Estar,F.size()-1,Estar);

                        E.erase(E.begin() + Estar);

                        double onesOut = arma::accu(colE);
                        double zerosOut = colE.n_elem - onesOut;

                        onesOutside += onesOut;
                        zerosInside -= zerosOut;

                        RemoveCubeColumn(Estar);
                    }
                    if (lossSub == LossB)
                    {
                        auto bicB = cube.slice(Bstar);

                        B.erase(B.begin() + Bstar);

                        double onesOut = arma::accu(bicB);
                        double zerosOut = bicB.n_elem - onesOut;

                        onesOutside += onesOut;
                        zerosInside -= zerosOut;

                        cube.shed_slice(Bstar);
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

            if (F.size() > 1 && E.size() > 1 && B.size() > 0)
            {
                std::shared_ptr<Bicluster> New = std::make_shared<Bicluster>(-1, F, E, dataMatrix->AverageCorrelationValue(F, E), nullptr);

                RetVal.push_back(New);
            }

            for(int b = B.size()-1; b >= 0; --b)
            {
                Biclusters.erase(Biclusters.begin()+B[b]);
            }
        }
        while (k < expectedBiClusterCount);
    }
    catch (std::exception& e)
    {
        qDebug() << "Exception " <<  e.what();
        //Logger.Log(String.Format("Error Message: {0}, StackTrace: {1}", ex.Message, ex.StackTrace), Common.LogTypes.Error);
    }

    return RetVal;
}

void TriClustering::RemoveCubeColumn(std::vector<int> colIndex)
{
    arma::ucube newCube = arma::ucube(cube.n_rows, cube.n_cols - colIndex.size(), cube.n_slices);

    //newCube.zeros(cube.n_rows - 1, cube.n_cols, cube.n_slices);

    for (uint b = 0; b < cube.n_slices; ++b)
    {
        for (uint f = 0; f < cube.n_rows; ++f)
        {
            int vecIndexE = 0;
            int remIndex = 0;

            for (uint e = 0; e < cube.n_cols; ++e)
            {
                if (e != colIndex[remIndex])
                {
                    newCube(f, vecIndexE, b) = cube(f, e, b);
                    vecIndexE++;
                }
                else
                {
                    if (remIndex < colIndex.size() - 1)
                        remIndex++;
                }
            }
        }
    }

    cube = newCube;

}

void TriClustering::RemoveCubeColumn(int colIndex)
{
    arma::ucube newCube = arma::ucube(cube.n_rows, cube.n_cols - 1, cube.n_slices);

    //newCube.zeros(cube.n_rows - 1, cube.n_cols, cube.n_slices);

    for (uint b = 0; b < cube.n_slices; ++b)
    {
        for (uint f = 0; f < cube.n_rows; ++f)
        {
            int vecIndexE = 0;

            for (uint e = 0; e < cube.n_cols; ++e)
            {
                if (e != colIndex)
                {
                    newCube(f, vecIndexE, b) = cube(f, e, b);
                    vecIndexE++;
                }
            }
        }
    }

    cube = newCube;

}

void TriClustering::RemoveCubeRow(std::vector<int> rowIndex)
{
    arma::ucube newCube = arma::ucube(cube.n_rows - rowIndex.size(), cube.n_cols, cube.n_slices);

    //newCube.zeros(cube.n_rows - 1, cube.n_cols, cube.n_slices);

    for (uint b = 0; b < cube.n_slices; ++b)
    {
        for (uint e = 0; e < cube.n_cols; ++e)
        {
            int vecIndexF = 0;
            int remIndex = 0;

            for (uint f = 0; f < cube.n_rows; ++f)
            {
                if (f != rowIndex[remIndex])
                {
                    newCube(vecIndexF, e, b) = cube(f, e, b);
                    vecIndexF++;
                }
                else
                {
                    if (remIndex < rowIndex.size()-1)
                        remIndex++;
                }
            }
        }
    }

    cube = newCube;

}

void TriClustering::RemoveCubeRow(int rowIndex)
{
    arma::ucube newCube = arma::ucube(cube.n_rows - 1, cube.n_cols, cube.n_slices);

    //newCube.zeros(cube.n_rows - 1, cube.n_cols, cube.n_slices);

    for (uint b = 0; b < cube.n_slices; ++b)
    {
        for (uint e = 0; e < cube.n_cols; ++e)
        {
            int vecIndexF = 0;

            for (uint f = 0; f < cube.n_rows; ++f)
            {
                if (f != rowIndex)
                {
                    newCube(vecIndexF, e, b) = cube(f, e, b);
                    vecIndexF++;
                }
            }
        }
    }

    cube = newCube;

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
                    cube(i, j, b) = 1;
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

    int outside = 0;
    int inside = 0;
    int zerosIn = 0;
    int onesOut = 0;
    double testSum = 0;

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
                    inside++;

                    if ((*cube)(f,e,b) == 0)
                    {
                        retVal += 1.0;
                        zerosIn++;
                    }
                }
                // Count ones outside selected area
                else
                {
                    outside++;

                    testSum += (*cube)(f,e,b);

                    if ((*cube)(f,e,b) == 1)
                    {
                        retVal += 1.0;
                        onesOut++;
                    }
                    else if ((*cube)(f,e,b) != 0)
                    {
                        qDebug() << (*cube)(f,e,b);
                    }
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
