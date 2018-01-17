#ifndef TRICLUSTERING_H
#define TRICLUSTERING_H

#include "ensembletask.h"
#include <QThreadPool>
#include <QMutex>
#include <exception>

class TriClustering : public EnsembleTask
{
    Q_OBJECT

public:

    arma::ucube cube;
    //arma::Cube<short> outcube;
    std::vector<int> trimF;
    std::vector<int> trimE;
    std::vector<int> trimB;


    TriClustering(std::shared_ptr<Matrix>& Vmatrix, int IdResult, double time, QString desc)
        : EnsembleTask(Vmatrix, Enums::Methods::TRICLUSTERING, IdResult, time, desc)
    {

    }

    //TriClustering(const TriClustering& Copy)
    //    : EnsembleTask(Copy)
    //{

    //}

    std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);

private:
    void BuildBinaryCube(const std::vector<std::shared_ptr<Bicluster>>& Biclusters, int dim1, int dim2);
    std::vector<std::shared_ptr<Bicluster>> DoTheTriClustering(std::vector<std::shared_ptr<Bicluster>> Biclusters);
    void RemoveCubeRow(int rowIndex);
    void RemoveCubeColumn(int colIndex);
    void RemoveCubeColumn(std::vector<int> colIndex);
    void RemoveCubeRow(std::vector<int> rowIndex);

};


class SingleLossWorker : public QRunnable
{
private:
    std::vector<double>::iterator pointer;
    std::vector<int> F;
    std::vector<int> E;
    std::vector<int> B;
    arma::ucube* cube;

public:
    SingleLossWorker(std::vector<double>::iterator pointer, const std::vector<int>& F, const std::vector<int>& E, const std::vector<int>& B, arma::ucube* cube)
        : pointer(pointer), F(F), E(E), B(B), cube(cube)
    {

    }

    void run();
};

#endif // TRICLUSTERING_H
