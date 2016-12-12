#include "dbtools.h"

std::vector<Label> DBTools::GetLabels(int idMatrix, int idLabelType)
{
    QSqlQuery query(db);

    QString queryText = "SELECT [id_label], [id_matrix], [id_label_type], [index], [value] FROM LABEL WHERE id_matrix = " + QString::number(idMatrix) + " AND id_label_type = " + QString::number(idLabelType) + " ORDER BY [index]";

    bool test = query.exec(queryText);

    query.execBatch();

    if (test)
    {
        //item.idFeature = query.lastInsertId().toInt();
    }
    else
    {
        qDebug() << query.lastError();
        qDebug() << query.executedQuery();
    }

    std::vector<Label> retVal;

    while(query.next())
    {
        int idLabel = query.value("id_label").toInt();
        int index = query.value("index").toInt();
        QString value = query.value("value").toString();

        retVal.emplace_back(idLabel, idMatrix, idLabelType, index, value);
    }

    return retVal;
}

bool DBTools::SaveLabels(std::vector<Label>& labels, int idMatrix)
{
    bool retVal = true;

    QSqlQuery query(db);

    query.exec("PRAGMA journal_mode = OFF");
    query.exec("PRAGMA synchronous = OFF");
    query.exec("PRAGMA locking_mode = EXCLUSIVE");
    query.exec("PRAGMA foreign_keys = OFF");
    query.exec("PRAGMA temp_store = MEMORY");

    QVariantList types;
    QVariantList matrices;
    QVariantList indexes;
    QVariantList values;

    QString queryText = "INSERT INTO LABEL ([id_matrix], [id_label_type], [index], [value]) VALUES (:matrix,:type,:index,:value);";

    query.prepare(queryText);

    for(Label item : labels)
    {
        if (item.idLabel > 0)
            continue;

        types.push_back(item.idLabelType);
        matrices.push_back(idMatrix);
        indexes.push_back(item.indexNbr);
        values.push_back(item.value);
    }

    //qDebug() << "add_table_data : " << db.transaction();

    query.bindValue(":type", types);
    query.bindValue(":matrix", matrices);
    query.bindValue(":index", indexes);
    query.bindValue(":value", values);

    //qDebug() << "add_table_data : " << db.commit();

    bool test = query.execBatch();

    if (test)
    {
        //item.idFeature = query.lastInsertId().toInt();
    }
    else
    {
        retVal = false;
        qDebug() << query.lastError();
        qDebug() << query.executedQuery();
    }

    return retVal;
}
