#ifndef DBTOOLS_H
#define DBTOOLS_H

#include <QString>
#include "nmf.h"
#include <QSqlDatabase>
#include "common.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

class DBTools
{
public:
    DBTools(const QString&);
    int SaveResult(std::shared_ptr<BiclusteringObject> taskToSave);
    int SaveMatrix(arma::mat matrixToSave, QString name, QString group, int type, int result);
    bool SaveBiclusters(std::vector<std::shared_ptr<Bicluster>>&, int idMatrix, int idResult);
    bool SaveFeatures(std::vector<FeatureResult>& features, int idResult);
    std::vector<std::shared_ptr<BiclusteringObject>> GetResults(int, int, int, int);
    std::shared_ptr<Matrix> GetMatrix(int);
    std::vector<std::shared_ptr<Bicluster>> GetBiclusters(int, int);
    bool SaveLabels(std::vector<Label>& labels, int idMatrix);
    std::vector<Label> GetLabels(int idMatrix, int idLabelType);

private:
    QString path;
    QSqlDatabase db;
    void CreateNewDatabase();
};

#endif // DBTOOLS_H
