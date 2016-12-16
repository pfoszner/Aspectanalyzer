#ifndef MATRIX_H
#define MATRIX_H

#include <QString>
#include "bicluster.h"
#include "label.h"
#include <QString>
#include <QTextStream>
#include <QDebug>
//#include <armadillo>
#include "/usr/local/include/armadillo"

class Matrix
{
public:
    Matrix(std::vector <QString>&);
    Matrix(const Matrix&);
    Matrix(int idMatrix, const arma::mat& data, const QString& name, const QString& group, const std::vector<std::shared_ptr<Bicluster>>& biclusters)
        : idMatrix(idMatrix), data(data), name(name), group(group), expectedBiClusters(biclusters)
    {
        expectedBiClusterCount = biclusters.size();
    }
    int idMatrix = -1;
    arma::mat data;
    QString name;
    QString group;
    uint expectedBiClusterCount;
    std::vector<std::shared_ptr<Bicluster>> expectedBiClusters;
    std::shared_ptr<double> AverageCorrelationValue(const std::vector<int>& clusterW, const std::vector<int>& clusterH);
    std::shared_ptr<double> AverageCorrelationValue(const arma::mat& Amatrix);
    std::vector<Label> rowLabels;
    std::vector<Label> columnLabels;
    std::vector<Label> classLabels;

public:
    void AddValue(double value);
};

#endif // MATRIX_H
