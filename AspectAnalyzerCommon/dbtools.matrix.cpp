#include "dbtools.h"

std::vector<int> DBTools::getGroupOfMatrices(QString group)
{
    QString queryString = "SELECT id_matrix FROM matrix WHERE [group] = '" + group + "'";

    std::vector<int> retVal;

    QSqlQuery query(db);

    query.exec(queryString);

    while (query.next())
    {
        int ID = query.value("id_matrix").toInt();

        retVal.push_back(ID);
    }

    return retVal;
}

void DBTools::GetMatrixTableList(QTableWidget *table)
{
    QSqlQuery query(db);

    QString queryText = "SELECT id_matrix, name, dim1, dim2, [group]  FROM matrix WHERE id_matrix_type = " + QString::number(Enums::MatrixType::V);

    query.exec(queryText);

    //int numOfRows = query.numRowsAffected();

    table->setColumnCount(5);

    QStringList labels;

    labels.append("Id Matrix");
    labels.append("Name");
    labels.append("Dimensions");
    labels.append("Group");
    labels.append("Load");

    table->setHorizontalHeaderLabels(labels);

    int row = 0;

    while (query.next())
    {
        table->setRowCount(row + 1);

        QTableWidgetItem *idMatrix = new QTableWidgetItem(query.value("id_matrix").toString());
        idMatrix->setFlags(idMatrix->flags() &  ~Qt::ItemIsEditable);
        QTableWidgetItem *name = new QTableWidgetItem(query.value("name").toString());
        name->setFlags(name->flags() &  ~Qt::ItemIsEditable);
        QTableWidgetItem *dim = new QTableWidgetItem(query.value("dim1").toString() + "x" + query.value("dim2").toString());
        dim->setFlags(dim->flags() &  ~Qt::ItemIsEditable);
        QTableWidgetItem *group = new QTableWidgetItem(query.value("group").toString());
        group->setFlags(group->flags() &  ~Qt::ItemIsEditable);

        table->setItem(row, 0, idMatrix);
        table->setItem(row, 1, name);
        table->setItem(row, 2, dim);
        table->setItem(row++, 3, group);

    }
}

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

    tmpFile.close();

    arma::mat data;

    data.load(filename.toStdString());

    //qDebug() << "Size: " << data.n_rows << ", " << data.n_cols;

    QFile::remove(filename);

    QString name = query.value("name").toString();

    QString group = query.value("group").toString();;

    std::vector<std::shared_ptr<Bicluster>> expectedBiclusters = GetBiclusters(idMatrix, -1);

    std::shared_ptr<int> ptrIdMatrix = std::make_shared<int>(idMatrix);

    retVal = std::make_shared<Matrix>(ptrIdMatrix, data, name, group, expectedBiclusters);

    retVal->rowLabels = GetLabels(idMatrix, Enums::LabelType::RowLabel);
    retVal->columnLabels = GetLabels(idMatrix, Enums::LabelType::ColumnLabel);

    return retVal;
}

void DBTools::GetMatrixData(int idResult, int idType, std::shared_ptr<NMF> result)
{
    QSqlQuery query(db);

    QString queryText = "SELECT [data] as rawdata FROM matrix WHERE id_result = " + QString::number(idResult) + " and id_matrix_type = " + QString::number(idType);

    query.exec(queryText);

    query.next();

    QByteArray rawData = query.value("rawdata").toByteArray();

    QDir dir = QDir::current();

    if (!dir.cd("tmp"))
    {
        dir.mkdir("tmp");
    }

    QString filename = "tmp/dataToInsert.mat";

    QFile::remove(filename);

    QFile tmpFile(filename);

    tmpFile.open(QIODevice::WriteOnly);

    auto writeResult = tmpFile.write(rawData);

    tmpFile.close();

    if (idType == Enums::MatrixType::W)
        result->WMatrix.load(filename.toStdString());
    else
        result->HMatrix.load(filename.toStdString());

    QFile::remove(filename);
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

    //std::ifstream file(filename.toStdString());

    //file.seekg(0, std::ios_base::end);

    //auto fileSize = file.tellg();

    //file.seekg(0, std::ios_base::beg);

    //file.open(QIODevice::ReadOnly);

    //qDebug() << rawData.size();

    //std::vector<char> rawData(fileSize);// = file.readAll();

    //file.read(rawData.data(), fileSize);

    //qDebug() << file.errorString();

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
        //qDebug() << query.lastInsertId();


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
