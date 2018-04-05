#include "dbtools.h"

std::vector<int> DBTools::GetResultsIDs(int m)
{
    std::vector<int> retVal;

    QString queryString = "SELECT id_result FROM result where id_matrix = " + QString::number(m) + " and id_method <> 8";

    //qDebug() << queryString;

    QSqlQuery query(db);

    query.exec(queryString);

    while (query.next())
    {
        int ID = query.value("id_result").toInt();

        retVal.push_back(ID);
    }

    return retVal;
}

int DBTools::GetResultsCount(int idResult, int VmatrixID, int idMethod, int DbK)
{
    QString queryString = "SELECT COUNT(RESULT.id_result) as ilosc FROM RESULT";

    idMethod++;

    if (idResult > 0)
        queryString += " WHERE RESULT.id_result = " + QString::number(idResult);
    else if (VmatrixID > 0 && idMethod > 0)
        queryString += " WHERE RESULT.id_matrix = " + QString::number(VmatrixID) + " and RESULT.id_method = " + QString::number(idMethod);
    else if (VmatrixID > 0)
        queryString += " WHERE RESULT.id_matrix = " + QString::number(VmatrixID);
    else if (idMethod > 0)
        queryString += " WHERE RESULT.id_method = " + QString::number(idMethod);

    if (DbK > 0)
        queryString += " AND RESULT.k = " + QString::number(DbK);

    queryString += " ORDER BY RESULT.id_result";

    //qDebug() << queryString;

    QSqlQuery query(db);

    query.exec(queryString);

    query.next();

    return query.value("ilosc").toInt();
}

std::vector<std::shared_ptr<BiclusteringObject>> DBTools::GetResults(int idResult, int VmatrixID, int idMethod, int DbK)
{
    std::vector<std::shared_ptr<BiclusteringObject>> retVal;

    QString queryString = "SELECT RESULT.id_result as id_result, RESULT.id_method as id_method, RESULT.time as time, RESULT.id_matrix as id_matrix, RESULT.desc as desc, FEATURE.id_feature as id_feature, FEATURE.value as value, FEATURE.id_feature_type as id_type, FEATURE.[index] as [index] FROM (RESULT LEFT OUTER JOIN FEATURE ON RESULT.id_result = FEATURE.id_result)";

    idMethod++;

    if (idResult > 0)
        queryString += " WHERE RESULT.id_result = " + QString::number(idResult);
    else if (VmatrixID > 0 && idMethod > 0)
        queryString += " WHERE RESULT.id_matrix = " + QString::number(VmatrixID) + " and RESULT.id_method = " + QString::number(idMethod);
    else if (VmatrixID > 0)
        queryString += " WHERE RESULT.id_matrix = " + QString::number(VmatrixID);
    else if (idMethod > 0)
        queryString += " WHERE RESULT.id_method = " + QString::number(idMethod);

    if (DbK > 0)
        queryString += " AND RESULT.k = " + QString::number(DbK);

    queryString += " ORDER BY RESULT.id_result";

    //qDebug() << queryString;

    QSqlQuery query(db);

    std::shared_ptr<BiclusteringObject> Result;

    bool error = query.exec(queryString);

    if (!error)
    {
        qDebug() << queryString;
    }

    while (query.next())
    {
        int ID = query.value("id_result").toInt();
        int method = query.value("id_method").toInt() - 1;
        double time = query.value("time").toDouble();
        int Vmatrix = query.value("id_matrix").toInt();
        QString desc = query.value("desc").toString();

        if (Result != nullptr && Result->idResult != ID)
        {
            if (Result->foundedBiclusters.size() == 0)
                Result->foundedBiclusters = GetBiclusters(*Result->dataMatrix->idMatrix, Result->idResult);

            std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(Result);

            if (tmpPtr != nullptr)
            {
                GetMatrixData(tmpPtr->idResult, Enums::MatrixType::W, tmpPtr);
                GetMatrixData(tmpPtr->idResult, Enums::MatrixType::H, tmpPtr);
            }
            retVal.push_back(Result);
            Result = nullptr;
        }

        if (Result == nullptr)
        {
            std::shared_ptr<Matrix> dataMatrix = GetMatrix(Vmatrix);

            if (   Enums::Methods(method) == Enums::Methods::PLSA
                || Enums::Methods(method) == Enums::Methods::LEAST_SQUARE_ERROR
                || Enums::Methods(method) == Enums::Methods::KULLBACK_LIEBER
                || Enums::Methods(method) == Enums::Methods::NonSmooth_KULLBACK_LIEBER
            )
                Result = std::make_shared<NMF>(dataMatrix, Enums::Methods(method), ID, time, desc);
            else
                Result = std::make_shared<BiclusteringObject>(dataMatrix, Enums::Methods(method), ID, time, desc);

            std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

            params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(Result->dataMatrix->expectedBiClusterCount));

            Result->ParseParameters(params);
        }

        if (!query.isNull("value"))
        {
            double Fvalue = query.value("value").toDouble();
            int type = query.value("id_type").toInt();
            int index = query.value("index").toInt();
            int IdFeature = query.value("id_feature").toInt();

            Result->features.emplace_back(Enums::FeatureType(type), Fvalue, index, IdFeature, ID, -1, -1);
        }
    }

    if (Result != nullptr)
    {
        if (Result->foundedBiclusters.size() == 0)
            Result->foundedBiclusters = GetBiclusters(*Result->dataMatrix->idMatrix, Result->idResult);

        std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(Result);

        if (tmpPtr != nullptr)
        {
            GetMatrixData(tmpPtr->idResult, Enums::MatrixType::W, tmpPtr);
            GetMatrixData(tmpPtr->idResult, Enums::MatrixType::H, tmpPtr);

            //qDebug() << tmpPtr->WMatrix.n_cols;
            //qDebug() << tmpPtr->WMatrix.n_rows;

            //qDebug() << tmpPtr->HMatrix.n_cols;
            //qDebug() << tmpPtr->HMatrix.n_rows;
        }

        retVal.push_back(Result);
        Result = nullptr;
    }

    return retVal;
}

int DBTools::SaveResult(std::shared_ptr<BiclusteringObject> taskToSave)
{
    int retVal = -1;

    if (taskToSave->dataMatrix->idMatrix == nullptr)
    {
        taskToSave->dataMatrix->idMatrix = std::make_shared<int>(-1);
    }

    //Save Matrix if not save
    if (*taskToSave->dataMatrix->idMatrix < 0)
    {
        *taskToSave->dataMatrix->idMatrix = SaveMatrix(taskToSave->dataMatrix->data, taskToSave->dataMatrix->name, taskToSave->dataMatrix->group, Enums::MatrixType::V, -1);
        SaveBiclusters(taskToSave->dataMatrix->expectedBiClusters, *taskToSave->dataMatrix->idMatrix, -1);
        SaveLabels(taskToSave->dataMatrix->rowLabels, *taskToSave->dataMatrix->idMatrix);
        SaveLabels(taskToSave->dataMatrix->columnLabels, *taskToSave->dataMatrix->idMatrix);
    }

    //save result row
    QSqlQuery query(db);

    QString queryText = "INSERT INTO RESULT ([id_method],[id_matrix],[k],[time],[desc]) VALUES (:method,:matrix,:bicNum,:time,:desc);";

    query.prepare(queryText);
    query.bindValue(":method", taskToSave->idMethod + 1);
    query.bindValue(":matrix", *taskToSave->dataMatrix->idMatrix);
    query.bindValue(":bicNum", (int)taskToSave->foundedBiclusters.size());
    query.bindValue(":time", taskToSave->time_spent);
    query.bindValue(":desc", taskToSave->desc);

    bool test = query.exec();

    if (test)
    {
        taskToSave->idResult = query.lastInsertId().toInt();
    }
    else
    {
        qDebug() << query.lastError();
        qDebug() << query.executedQuery();
    }

    //save additional matrices if any

    std::shared_ptr<NMF> tmpPtr = std::dynamic_pointer_cast<NMF>(taskToSave);

    if (tmpPtr != nullptr)
    {
        SaveMatrix(tmpPtr->WMatrix, "", "", Enums::MatrixType::W, taskToSave->idResult);
        SaveMatrix(tmpPtr->HMatrix, "", "", Enums::MatrixType::H, taskToSave->idResult);
    }

    //save founded biclusters
    SaveBiclusters(taskToSave->foundedBiclusters, *taskToSave->dataMatrix->idMatrix, taskToSave->idResult);

    //save features
    SaveFeatures(taskToSave->features, taskToSave->idResult, -1, -1);

    return retVal;
}
