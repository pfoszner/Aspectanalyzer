#ifndef MATRIX_H
#define MATRIX_H

#include <QString>
#include "bicluster.h"
#include "label.h"
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <armadillo>
#include <QtGui/QImage>
//#include "/usr/local/include/armadillo"

class Matrix
{


public:
    Matrix(int id, const arma::mat& data) : data(data)
    {
        idMatrix = std::make_shared<int>(id);
    }

    Matrix(QString filename);
    Matrix(std::vector <QString>&);
    Matrix(const Matrix&);
    Matrix(std::shared_ptr<int>& idMatrix, const arma::mat& data, const QString& name, const QString& group, const std::vector<std::shared_ptr<Bicluster>>& biclusters)
        : idMatrix(idMatrix), data(data), name(name), group(group), expectedBiClusters(biclusters)
    {
        expectedBiClusterCount = (uint)biclusters.size();
    }
    std::shared_ptr<int> idMatrix;
    arma::mat data;
    QString name;
    QString group;
    uint expectedBiClusterCount;
    std::vector<std::shared_ptr<Bicluster>> expectedBiClusters;

    double CalculateQualityMeasure(Enums::FeatureType measure, const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    std::vector<Label> rowLabels;
    std::vector<Label> columnLabels;
    std::vector<Label> classLabels;

public:
    void AddValue(double value);
    void WriteAsImage(QString filename);

private:
    void LoadFromDataList(std::vector <QString>& sdata);
    void LoadSoftFile(QString& filepath);
    void LoadDatasetFile(QString& filepath);

    double AverageCorrelationValue(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    double AverageCorrelationValue(const arma::mat& Amatrix);
    double MeanSquaredResidue(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    double MeanSquaredResidue(const arma::mat& Amatrix);
    double ScalingMeanSquaredResidue(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    double ScalingMeanSquaredResidue(const arma::mat& Amatrix);
    double AverageSpearmansRank(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    double AverageSpearmansRank(const arma::mat& Amatrix);
    double Variance(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    double Variance(const arma::mat& Amatrix);



};
#endif // MATRIX_H
