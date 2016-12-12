#include "dbtools.h"

std::shared_ptr<Matrix> DBTools::GetMatrix(int idMatrix)
{
    std::shared_ptr<Matrix> retVal;

    QSqlQuery query(db);

    QString queryText = "SELECT * FROM matrix WHERE id_matrix = " + QString::number(idMatrix);

    query.exec(queryText);

    query.next();

    QByteArray rawData = query.value("data").toByteArray();

    QDir dir = QDir::current();

    if (!dir.cd("tmp"))
    {
        dir.mkdir("tmp");
    }

    QString filename = "tmp/dataToInsert.mat";

    QFile tmpFile(filename);

    tmpFile.open(QIODevice::WriteOnly);

    tmpFile.write(rawData);

    arma::mat data;

    data.load(filename.toStdString());

    QFile::remove(filename);

    QString name = query.value("name").toString();

    QString group = query.value("group").toString();;

    std::vector<std::shared_ptr<Bicluster>> expectedBiclusters = GetBiclusters(idMatrix, -1);

    retVal = std::make_shared<Matrix>(idMatrix, data, name, group, expectedBiclusters);

    retVal->rowLabels = GetLabels(idMatrix, Enums::LabelType::RowLabel);
    retVal->columnLabels = GetLabels(idMatrix, Enums::LabelType::ColumnLabel);

    return retVal;
}

int DBTools::SaveMatrix(arma::mat matrixToSave, QString name, QString group, int type, int result)
{
    int retVal = -1;
    
    QSqlQuery query(db);
    
    QString queryText;
    
    QString error;

    QDir dir = QDir::current();

    if (!dir.cd("tmp"))
    {
        dir.mkdir("tmp");
    }

    QString filename = "tmp/dataToInsert.mat";

    matrixToSave.save(filename.toStdString());

    QFile file(filename);

    file.open(QIODevice::ReadOnly);

    QByteArray rawData = file.readAll();

    QFile::remove(filename);

    queryText = "INSERT INTO matrix (id_matrix_type, name, dim1, dim2, [group], id_result, [data]) VALUES (:type, :name, :dim1, :dim2, :group, :result, :rawdata)";
    query.prepare(queryText);
    query.bindValue(":type", type);
    query.bindValue(":name", name);
    query.bindValue(":dim1", matrixToSave.n_rows);
    query.bindValue(":dim2", matrixToSave.n_cols);
    query.bindValue(":group", group);
    if (result > 0)
        query.bindValue(":result", result);
    else
        query.bindValue(":result", QVariant(QVariant::Int));
    query.bindValue(":rawdata", rawData);

    bool test = query.exec();

    if (test)
    {
        retVal = query.lastInsertId().toInt();
        qDebug() << query.lastInsertId();


//       double test1 = matrixToSave(40,40);

//       query.exec("SELECT [data] FROM matrix WHERE id_matrix = " + QString::number(retVal));

//       query.next();

//       QByteArray test2 = query.value(0).toByteArray();

//       QFile test3(filename);

//       test3.open(QIODevice::WriteOnly);

//       test3.write(test2);

//       arma::mat test4;

//       test4.load(filename.toStdString());

//       QFile::remove(filename);

//       double test5 = test4(40,40);
    }
    else
    {
        error = query.lastError().driverText();
        qDebug() << query.lastError();
        qDebug() << query.executedQuery();
    }

    
    return retVal;
}