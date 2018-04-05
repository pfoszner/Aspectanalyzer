#include "dbtools.h"

DBTools::DBTools(const QString &path) : path(path)
{
    bool createNewDB = false;

    if (!CheckIfFileExists(path))
    {
        createNewDB = true;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(path);

    if (!db.open())
    {
        qDebug() << "Error: conection with database fail!";
    }
    else
    {
        qDebug() << "Connection sucessfull";
    }

    if (createNewDB)
    {
        CreateNewDatabase();
    }
}

void DBTools::CreateNewDatabase()
{
    QSqlQuery query;
    bool test = query.exec("BEGIN TRANSACTION; ");
               test = query.exec("CREATE TABLE `result` ("
                  " `id_result`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                  " `id_method`	INTEGER NOT NULL,"
                  " `id_matrix`	INTEGER NOT NULL,"
                  " `time`	REAL NOT NULL,"
                  " `k`	INTEGER NOT NULL,"
                  " `desc`	TEXT NOT NULL,"
                  " FOREIGN KEY(id_matrix) REFERENCES matrix(id_matrix)"
               "); ");
               test = query.exec("CREATE TABLE \"property_type\" ("
                  " `id_property_type`	INTEGER NOT NULL UNIQUE,"
                  " `name`	TEXT NOT NULL,"
                  " PRIMARY KEY(`id_property_type`)"
               "); ");
               test = query.exec("CREATE TABLE `method` ("
                  " `id_method`	INTEGER NOT NULL UNIQUE,"
                  " `name`	TEXT NOT NULL,"
                  " PRIMARY KEY(`id_method`)"
               ");");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (1,'PLSA'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (2,'KULLBACK_LIEBER'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (3,'LEAST_SQUARE_ERROR'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (4,'NonSmooth_KULLBACK_LIEBER'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (5,'FABIA'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (6,'FABIAS'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (7,'QUBIC'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (8,'CONSENSUS'); ");
               test = query.exec("INSERT INTO `method` (id_method,name) VALUES (9,'TRICLUSTERING'); ");
               test = query.exec("CREATE TABLE \"matrix_type\" ("
                 "  `id_matrix_type`	INTEGER NOT NULL UNIQUE,"
                 "  `name`	TEXT NOT NULL,"
                 "  PRIMARY KEY(`id_matrix_type`)"
               ");");
               test = query.exec("INSERT INTO `matrix_type` (id_matrix_type,name) VALUES (1,'Data Matrix'); ");
               test = query.exec("INSERT INTO `matrix_type` (id_matrix_type,name) VALUES (2,'W Matrix'); ");
               test = query.exec("INSERT INTO `matrix_type` (id_matrix_type,name) VALUES (3,'H Matrix'); ");
               test = query.exec("CREATE TABLE \"matrix_property\" ("
                  " `id_matrix_property`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                  " `id_matrix`	INTEGER NOT NULL,"
                  " `id_property_type`	INTEGER NOT NULL,"
                  " `value`	TEXT NOT NULL"
               ");");
               test = query.exec("CREATE TABLE \"matrix\" ("
                 "  `id_matrix`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                 "  `id_matrix_type`	INTEGER NOT NULL,"
                 "  `name`	TEXT NOT NULL,"
                 "  `dim1`	INTEGER NOT NULL,"
                 "  `dim2`	INTEGER NOT NULL,"
                 "  `group`	TEXT NOT NULL,"
                 "  `id_result`  INTEGER,"
                 "  `data`  LARGEBLOB,"
                 " FOREIGN KEY(id_result) REFERENCES result(id_result)"
               ");");
               test = query.exec("CREATE TABLE `label_type` ("
               "    `id_label_type`	INTEGER NOT NULL UNIQUE,"
               "    `name`	TEXT NOT NULL,"
               "    PRIMARY KEY(`id_label_type`)"
               ");");
               test = query.exec("INSERT INTO `label_type` (id_label_type,name) VALUES (1,'Label Dimension 1'); ");
               test = query.exec("INSERT INTO `label_type` (id_label_type,name) VALUES (2,'Label Dimension 2'); ");
               test = query.exec("INSERT INTO `label_type` (id_label_type,name) VALUES (3,'Class'); ");
               test = query.exec("CREATE TABLE \"label\" ("
               "    `id_label`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "    `id_label_type`	INTEGER NOT NULL,"
               "    `id_matrix`	INTEGER NOT NULL,"
               "    `index`	INTEGER NOT NULL,"
               "    `value`	TEXT NOT NULL"
               ");");
               test = query.exec("CREATE TABLE `feature_type` ("
               "    `id_feature_type`	INTEGER NOT NULL UNIQUE,"
               "    `name`	TEXT NOT NULL,"
               "    PRIMARY KEY(`id_feature_type`)"
               ");");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (1,'SimilarityJaccard'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (2,'SimilarityLevenstine'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (3,'MSR'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (4,'ACV'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (5,'ASR'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (6,'Divergence'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (7,'BiclusterCount'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (8,'AverageACV'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (9,'AverageSimilarity'); ");
               test = query.exec("INSERT INTO `feature_type` (id_feature_type,name) VALUES (10,'Consensus Extraction Type'); ");
               test = query.exec("CREATE TABLE \"feature\" ("
               "    `id_feature`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "    `id_result`	INTEGER,"
               "    `id_matrix`	INTEGER,"
               "    `id_bicluster`	INTEGER,"
               "    `id_feature_type`	INTEGER NOT NULL,"
               "    `index`	INTEGER NOT NULL DEFAULT 0,"
               "    `value`	REAL NOT NULL,"
               " FOREIGN KEY(id_matrix) REFERENCES matrix(id_matrix),"
               " FOREIGN KEY(id_result) REFERENCES result(id_result),"
               " FOREIGN KEY(id_bicluster) REFERENCES bicluster(id_bicluster)"
               ");");
               test = query.exec("CREATE TABLE \"bicluster\" ("
               "    `id_bicluster`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "    `id_matrix`	INTEGER NOT NULL,"
               "    `id_result`	INTEGER,"
               "    `index_nbr`	INTEGER NOT NULL,"
               "    `data`	TEXT NOT NULL"
               ");");
               test = query.exec("CREATE INDEX `result_id_matrix` ON `result` ("
               "    `id_matrix`	ASC"
               ");");
               test = query.exec("CREATE INDEX `feature_id_result` ON `feature` ("
               "    `id_result`	ASC"
               ");");
               test = query.exec("CREATE INDEX `bicluster_id_result` ON `bicluster` ("
               "    `id_result`	ASC"
               ");");
               test = query.exec("PRAGMA journal_mode = OFF");
               test = query.exec("PRAGMA synchronous = OFF");
               test = query.exec("PRAGMA locking_mode = EXCLUSIVE");
               test = query.exec("PRAGMA foreign_keys = OFF");
               test = query.exec("PRAGMA temp_store = MEMORY");
               test = query.exec("PRAGMA SQLITE_MAX_LENGTH = 2147483647");
               test = query.exec("COMMIT;");

     if (!test)
         throw("AAAAAAAAAAAAAA DatabasePanic - unable to create database file");
}
