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
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSignalMapper>

class DBTools
{

public:
    DBTools(const QString&);
    int SaveResult(std::shared_ptr<BiclusteringObject> taskToSave);
    int SaveMatrix(arma::mat matrixToSave, QString name, QString group, int type, int result);
    bool SaveBiclusters(std::vector<std::shared_ptr<Bicluster>>&, int idMatrix, int idResult);
    bool SaveFeatures(std::vector<FeatureResult>& features, int idResult, int idBicluster, int idMatrix);
    std::vector<std::shared_ptr<BiclusteringObject>> GetResults(int idResult, int VmatrixID, int idMethod, int DbK);
    int GetResultsCount(int idResult, int VmatrixID, int idMethod, int DbK);
    std::shared_ptr<Matrix> GetMatrix(int);
    std::vector<std::shared_ptr<Bicluster>> GetBiclusters(int, int);
    bool SaveLabels(std::vector<Label>& labels, int idMatrix);
    std::vector<Label> GetLabels(int idMatrix, int idLabelType);
    void GetMatrixData(int, int, std::shared_ptr<NMF>);
    void GetMatrixTableList(QTableWidget *table);
    std::vector<int> GetResultsIDs(int m);
    std::vector<int> getGroupOfMatrices(QString group);

private:
    QString path;
    QSqlDatabase db;
    void CreateNewDatabase();
};

#endif // DBTOOLS_H
