#include "matrix.h"

Matrix::Matrix(QString filepath)
{
    QFileInfo fi(filepath);

    QString extension = fi.suffix();

    if (extension == "vmatrix"){

        QFile file(filepath);
        if(!file.open(QIODevice::ReadOnly))
        {
            //QMessageBox::information(0, "error", file.errorString());
        }
        else
        {

            QTextStream in(&file);

            std::vector <QString> vec;

            while(!in.atEnd()) {
                QString line = in.readLine().trimmed();

                vec.push_back(line);
            }

            LoadFromDataList(vec);



            file.close();
        }
    }
    else if (extension == "soft"){
            LoadSoftFile(filepath);
    }
}

Matrix::Matrix(std::vector <QString>& sdata)
{
    LoadFromDataList(sdata);
}

void Matrix::LoadSoftFile(QString& filepath)
{
    idMatrix = std::make_shared<int>(-1);

    QFile file(filepath);

    if(!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
    }
    else
    {
        QTextStream in(&file);

        bool waitForData = true;
        bool header = true;

        int rowIndex = 0;

        while(!in.atEnd()) {

            QString line = in.readLine().trimmed();

            if (waitForData){
                if (line == "!dataset_table_begin"){
                    waitForData = false;
                }
            }
            else{
                if (header){

                    QStringList tmp = line.split('\t');

                    for(int i = 2; i < tmp.size(); ++i)
                    {
                        columnLabels.emplace_back(-1, -1, Enums::LabelType::ColumnLabel, i-2, tmp[i]);
                    }

                    header = false;
                }
                else{
                    if (line == "!dataset_table_end")
                        break;
                    else{

                        QStringList tmp = line.split('\t');

                        rowLabels.emplace_back(-1, -1, Enums::LabelType::RowLabel, rowIndex++, tmp[1]);

                        arma::vec row = arma::zeros<arma::vec>(tmp.size()-2);

                        std::vector<int> nullVals;

                        double averageInRow = 0;
                        int notNullNum = 0;

                        for(int i = 2; i < tmp.size(); ++i)
                        {
                            if (tmp[i] != "null"){
                                bool ok;

                                double value = tmp[i].trimmed().toDouble(&ok);

                                if (!ok)
                                {
                                    value = tmp[i].replace(",",".").toDouble(&ok);

                                    if (!ok)
                                    {
                                        qDebug() << "Conversion gone wrong. Value: \"" << tmp[i] << "\"";
                                    }
                                }

                                row(i - 2) = value;

                                averageInRow += value;
                                notNullNum++;
                            }
                            else{
                                nullVals.push_back(i-2);
                            }
                        }

                        if (notNullNum > 0){

                            if (nullVals.size() > 0){

                                averageInRow = averageInRow / notNullNum;

                                for(int n : nullVals)
                                {
                                    row[n] = averageInRow;
                                }
                            }

                            data = arma::join_cols(data, row.t());
                        }
                    }

                }
            }
        }
    }
}

void Matrix::LoadFromDataList(std::vector <QString>& sdata)
{
    idMatrix = std::make_shared<int>(-1);

    QString separator = "\t";

    QString test = sdata[0];

    for(QChar c : test)
    {
        if (c == '\t' || c == ';')
        {
            separator = c;
            break;
        }
    }

    QStringList tmp = sdata[0].split(separator, QString::SkipEmptyParts);

    test = sdata[1];

    for(QChar c : test)
    {
        if (c == '\t' || c == ';')
        {
            separator = c;
            break;
        }
    }

    if (sdata.back().size() == 0)
    {
        sdata.erase(sdata.end()-1);
    }

    this->name = tmp[1];
    this->group = tmp[2];
    this->expectedBiClusterCount = tmp[0].toInt();

    bool containsLabels = tmp[3].compare("True", Qt::CaseInsensitive) == 0;

    for(uint i = 1; i <= expectedBiClusterCount; ++i)
    {
        QStringList bicRow = sdata[i].split(separator);

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

        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(-1, cluster1, cluster2, nullptr, nullptr);
        expectedBiClusters.push_back(NewBic);
    }

    int startMainLoop = 1;

    if (containsLabels)
    {
        QStringList headerRow = sdata[startMainLoop + expectedBiClusterCount].split(separator);

        for(int i = 1; i < headerRow.size(); ++i)
        {
            columnLabels.emplace_back(-1, -1, Enums::LabelType::ColumnLabel, i-1, headerRow[i].trimmed());
        }

        startMainLoop = 2;
    }

    for(uint i = startMainLoop + expectedBiClusterCount; i < sdata.size(); ++i)
    {
        QStringList row = sdata[i].split(separator);

        int startInnerLoop = 0;

        if (containsLabels)
        {
            rowLabels.emplace_back(-1, -1, Enums::LabelType::RowLabel, i, row[0].trimmed());
            startInnerLoop = 1;
        }

        if (i == startMainLoop + expectedBiClusterCount)
        {
            data = arma::zeros<arma::mat>(sdata.size() - startMainLoop - expectedBiClusterCount, row.size() - startInnerLoop);
        }

        for(int j = startInnerLoop; j < row.size(); ++j)
        {
            bool ok;

            double value = row[j].trimmed().toDouble(&ok);

            if (!ok)
            {
                value = row[j].replace(",",".").toDouble(&ok);

                if (!ok)
                {
                    QString testValue = row[j];
                    qDebug() << "Conversion gone wrong. Value: \"" << row[j] << "\"";
                }
            }

            data(i - startMainLoop - expectedBiClusterCount, j-startInnerLoop) = value;
        }
    }

    for(auto bic : expectedBiClusters)
    {
        bic->ACV = AverageCorrelationValue(bic->cluster1, bic->cluster2);
    }

    double Min = data.min();
    double Max = data.max();

    if (Min == Max)
    {
        qDebug() << "Something Wrong with matrix";
    }
}


Matrix::Matrix(const Matrix &copy) : idMatrix(copy.idMatrix), data(copy.data), name(copy.name), group(copy.group), expectedBiClusterCount(copy.expectedBiClusterCount), expectedBiClusters(copy.expectedBiClusters), rowLabels(copy.rowLabels), columnLabels(copy.columnLabels), classLabels(copy.classLabels)
{
    /*expectedBiClusters = copy.expectedBiClusters;
    data = copy.data;
    expectedBiClusterCount = copy.expectedBiClusterCount;
    group = copy.group;
    name = copy.name;*/
}


void Matrix::AddValue(double value)
{
    data = data + value;
}

std::shared_ptr<double> Matrix::AverageCorrelationValue(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    std::shared_ptr<double> retVal;

    arma::mat Amatrix = arma::zeros<arma::mat>(clusterW.size(), clusterH.size());

    for (uint i = 0; i < clusterH.size(); ++i)
    {
        for (uint j = 0; j < clusterW.size(); ++j)
        {
            try
            {
                Amatrix(j, i) = data(clusterW[j], clusterH[i]);
            }
            catch(...)
            {
                qDebug() << "Panic. ACV part errror: " << clusterW[j] << "," << clusterH[i];
            }
        }
    }

    retVal = AverageCorrelationValue(Amatrix);

    return retVal;
}

std::shared_ptr<double> Matrix::AverageCorrelationValue(const arma::mat& Amatrix)
{
    int dim1 = Amatrix.n_rows;
    int dim2 = Amatrix.n_cols;

    double RowValue = 0;

    double iAverage;
    double jAverage;
    double nominator;
    double denominator;

    int div = 0;

    for (int i = 0; i < dim1 - 1; ++i)
    {
        arma::rowvec iRow = Amatrix.row(i);
        iAverage = arma::mean(iRow);
        double denominator1 = arma::accu((iRow - iAverage) % (iRow - iAverage));

        denominator1 = std::sqrt(denominator1);

        for (int j = i + 1; j < dim1; ++j)
        {
            arma::rowvec jRow = Amatrix.row(j);

            jAverage = arma::mean(jRow);

            nominator = arma::accu((iRow - iAverage) % (jRow - jAverage));

            double denominator2 = arma::accu((jRow - jAverage) % (jRow - jAverage));
            denominator = denominator1 * std::sqrt(denominator2);

            if (denominator != 0)
                RowValue += std::abs(nominator / denominator);
            else
                RowValue += 1;

            div++;
        }
    }

    RowValue /= div;

    double ColumnValue = 0;

    div = 0;

    for (int i = 0; i < dim2 - 1; ++i)
    {
        arma::colvec iCol = Amatrix.col(i);
        iAverage = arma::mean(iCol);
        double denominator1 = arma::accu((iCol - iAverage) % (iCol - iAverage));
        denominator1 = std::sqrt(denominator1);

        for (int j = i + 1; j < dim2; ++j)
        {
            arma::colvec jCol = Amatrix.col(j);

            jAverage = arma::mean(jCol);

            nominator = arma::accu((iCol - iAverage) % (jCol - jAverage));

            double denominator2 = arma::accu((jCol - jAverage) % (jCol - jAverage));
            denominator = denominator1 * std::sqrt(denominator2);

            if (denominator != 0)
                ColumnValue += std::abs(nominator / denominator);
            else
                ColumnValue += 1;

            div++;
        }
    }

    ColumnValue /= div;

    if (RowValue > ColumnValue)
        return std::make_shared<double>(RowValue);
    else
        return std::make_shared<double>(ColumnValue);
}
