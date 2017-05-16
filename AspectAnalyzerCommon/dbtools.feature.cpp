#include "dbtools.h"

bool DBTools::SaveFeatures(std::vector<FeatureResult>& features, int idResult)
{
    bool retVal = true;

    QSqlQuery query(db);

    query.exec("PRAGMA journal_mode = OFF");
    query.exec("PRAGMA synchronous = OFF");
    query.exec("PRAGMA locking_mode = EXCLUSIVE");
    query.exec("PRAGMA foreign_keys = OFF");
    query.exec("PRAGMA temp_store = MEMORY");

    QVariantList types;
    QVariantList results;
    QVariantList indexes;
    QVariantList values;

    QString queryText = "INSERT INTO FEATURE ([id_feature_type],[id_result],[index],[value]) VALUES (:type,:result,:index,:value);";

    query.prepare(queryText);


    for(FeatureResult item : features)
    {
        if (item.idFeature > 0)
            continue;

        types.push_back(item.type);
        results.push_back(idResult);
        indexes.push_back(item.indexNbr);
        values.push_back(item.value);
    }

    //qDebug() << "add_table_data : " << db.transaction();

    query.bindValue(":type", types);
    query.bindValue(":result", results);
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
