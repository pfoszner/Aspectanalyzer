#include "dbtools.h"

std::vector<std::shared_ptr<Bicluster>> DBTools::GetBiclusters(int idMatrix, int idResult)
{
    std::vector<std::shared_ptr<Bicluster>> retVal;

    QString queryText;

    QSqlQuery query(db);

    if (idResult < 0)
        queryText = "SELECT * FROM BICLUSTER WHERE id_matrix = " + QString::number(idMatrix) + " AND id_result is null";
    else
        queryText = "SELECT * FROM BICLUSTER WHERE id_result = " + QString::number(idResult);

    queryText += " ORDER BY id_bicluster";

    query.exec(queryText);

    while(query.next())
    {
        QString rawData = query.value("data").toString();
        std::shared_ptr<double> ACV = std::make_shared<double>(query.value("ACV").toDouble());
        std::shared_ptr<double> similarity = std::make_shared<double>(query.value("similarity").toDouble());

        QStringList bicRow = rawData.split("\t");

        std::vector<int> cluster1;
        std::vector<int> cluster2;

        int itemsToLoad = bicRow[0].toInt();

        bool first = true;

        for(int j = 1; j < bicRow.size(); ++j)
        {
            if (itemsToLoad < 0)
            {
                itemsToLoad = bicRow[j].toInt();
                first = false;
                continue;
            }

            int item = bicRow[j].toInt();

            if (first)
                cluster1.push_back(item);
            else
                cluster2.push_back(item);

            itemsToLoad--;

            if (itemsToLoad == 0)
                itemsToLoad = -1;
        }

        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(idResult, cluster1, cluster2, ACV, similarity);
        retVal.push_back(NewBic);

    }

    return retVal;
}

bool DBTools::SaveBiclusters(std::vector<std::shared_ptr<Bicluster>>& biclusters, int idMatrix, int idResult)
{
    bool retVal = true;

    QSqlQuery query(db);

    int index = 0;

    for(std::shared_ptr<Bicluster> bic : biclusters)
    {
        if (bic->idBicluster > 0)
            continue;

        QString queryText = "INSERT INTO BICLUSTER ([id_matrix],[id_result],[index_nbr],[similarity],[ACV],[data]) VALUES (:matrix,:result,:index,:simi,:acv,:data);";

        query.prepare(queryText);

        query.bindValue(":matrix", idMatrix);

        if (idResult > 0)
            query.bindValue(":result", idResult);
        else
            query.bindValue(":result", QVariant(QVariant::Int));

        query.bindValue(":index", index++);

        if (bic->similarity != nullptr)
            query.bindValue(":simi", *bic->similarity);
        else
            query.bindValue(":simi", QVariant(QVariant::Double));

        if (bic->ACV != nullptr)
            query.bindValue(":acv", *bic->ACV);
        else
            query.bindValue(":acv", QVariant(QVariant::Double));

        QString data = QString::number(bic->cluster1.size());

        for(int item : bic->cluster1)
        {
            data += "\t" + QString::number(item);
        }

        data += "\t" + QString::number(bic->cluster2.size());

        for(int item : bic->cluster2)
        {
            data += "\t" + QString::number(item);
        }

        query.bindValue(":data", data);

        bool test = query.exec();

        if (test)
        {
            bic->idBicluster = query.lastInsertId().toInt();
        }
        else
        {
            retVal = false;
            qDebug() << query.lastError();
            qDebug() << query.executedQuery();
        }
    }

    return retVal;
}
