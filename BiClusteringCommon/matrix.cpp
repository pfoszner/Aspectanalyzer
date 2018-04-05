#include "matrix.h"

using namespace arma;

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
    else if (extension == "dataset"){
            LoadDatasetFile(filepath);
    }
}



Matrix::Matrix(std::vector <QString>& sdata)
{
    LoadFromDataList(sdata);
}

void Matrix::LoadDatasetFile(QString& filepath)
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

        bool header = true;

        int rowIndex = 0;

        while(!in.atEnd()) {

            QString line = in.readLine().trimmed();

            if (header){

                QStringList tmp = line.split('\t');

                for(int i = 1; i < tmp.size(); ++i)
                {
                    Label newItem(-1, -1, (int)Enums::LabelType::ColumnLabel, i-1, tmp[i]);
                    columnLabels.push_back(newItem);
                }

                header = false;
            }
            else{
                QStringList tmp = line.split('\t');

                rowLabels.emplace_back(-1, -1, Enums::LabelType::RowLabel, rowIndex++, tmp[0]);

                arma::vec row = arma::zeros<arma::vec>(tmp.size()-1);

                for(int i = 1; i < tmp.size(); ++i)
                {
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

                        row(i - 1) = value;
                }

                data = arma::join_cols(data, row.t());

            }
        }
    }
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

        std::shared_ptr<Bicluster> NewBic = std::make_shared<Bicluster>(-1, cluster1, cluster2);
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
            data = arma::zeros<arma::mat>((uint)sdata.size() - startMainLoop - expectedBiClusterCount, row.size() - startInnerLoop);
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
        bic->SetFeature(Enums::FeatureType::ACV, AverageCorrelationValue(bic->cluster1, bic->cluster2));
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

double Matrix::AverageSpearmansRank(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    double retVal;

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)clusterW.size(), (uint)clusterH.size());

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

    retVal = AverageSpearmansRank(Amatrix);

    return retVal;
}

rowvec getSpearmanRanks(const rowvec X)
{
    rowvec retVal = zeros<rowvec>(X.n_elem);

    for(uint i = 0; i < X.n_elem; ++i)
    {
        double val = X(i);
        int count = 0;

        for(uint j = 0; j < X.n_elem; ++j)
        {
            if (X(j) <= val)
               count++;
        }

        retVal(i) = count;
    }

    return retVal;
}

double Matrix::Variance(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    double retVal;

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)clusterW.size(), (uint)clusterH.size());

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

    retVal = Variance(Amatrix);

    return retVal;
}

double Matrix::Variance(const arma::mat& Amatrix)
{
    double retVal = 0;

    int dim1 = Amatrix.n_rows;
    int dim2 = Amatrix.n_cols;

    double AIJ = 0;

    for (int i = 0; i < dim1; ++i)
    {
        for (int j = 0; j < dim2; ++j)
        {
            AIJ += Amatrix[i, j];
        }
    }

    AIJ /= dim1 * dim2;

    for (int i = 0; i < dim1; ++i)
    {
        for (int j = 0; j < dim2; ++j)
        {
            retVal += (Amatrix[i, j] - AIJ) * (Amatrix[i, j] - AIJ);
        }
    }

    return retVal;
}

double Matrix::AverageSpearmansRank(const arma::mat& Amatrix)
{
    int dim1 = Amatrix.n_rows;
    int dim2 = Amatrix.n_cols;

    double RowValue = 0;

    int div = 0;

    for (int i = 0; i < dim1 - 1; ++i)
    {
        rowvec iRow = Amatrix.row(i);

        rowvec iRank = getSpearmanRanks(iRow);

        double iStd = stddev(iRank);

        double iMean = mean(iRank);

        rowvec iRankMinusMean = iRank - iMean;

        for (int j = i + 1; j < dim1; ++j)
        {
            rowvec jRow = Amatrix.row(j);

            rowvec jRank = getSpearmanRanks(jRow);

            double jStd = stddev(jRank);

            double jMean = mean(jRank);

            rowvec jRankMinusMean = jRank - jMean;

            double covariance = accu(iRankMinusMean % jRankMinusMean) / (iRow.n_elem - 1);

            double denominator = iStd * jStd;

            if (denominator != 0)
                RowValue += covariance / denominator;
            else
                RowValue += 0;

            div++;
        }
    }

    RowValue /= div;

    div= 0;

    double ColumnValue = 0;

    for (int i = 0; i < dim2 - 1; ++i)
    {
        colvec iCol = Amatrix.col(i);

        colvec iRank = getSpearmanRanks(iCol.t()).t();

        double iStd = stddev(iRank);

        double iMean = mean(iRank);

        colvec iRankMinusMean = iRank - iMean;

        for (int j = i + 1; j < dim2; ++j)
        {
            colvec jCol = Amatrix.col(j);

            colvec jRank = getSpearmanRanks(jCol.t()).t();

            double jStd = stddev(jRank);

            double jMean = mean(jRank);

            colvec jRankMinusMean = jRank - jMean;

            double covariance = accu(iRankMinusMean % jRankMinusMean) / (iCol.n_elem - 1);

            double denominator = iStd * jStd;

            if (denominator != 0)
                ColumnValue += covariance / denominator;
            else
                ColumnValue += 1;

            div++;
        }
    }

    ColumnValue /= div;

    if (abs(RowValue) > abs(ColumnValue))
        return abs(RowValue);
    else
        return abs(ColumnValue);
}

double Matrix::ScalingMeanSquaredResidue(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    double retVal;

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)clusterW.size(), (uint)clusterH.size());

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

    retVal = ScalingMeanSquaredResidue(Amatrix);

    return retVal;
}

double Matrix::MeanSquaredResidue(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    double retVal;

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)clusterW.size(), (uint)clusterH.size());

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

    retVal = MeanSquaredResidue(Amatrix);

    return retVal;
}

double Matrix::ScalingMeanSquaredResidue(const arma::mat& Amatrix)
{
    double retVal = 0;

    int dim1 = Amatrix.n_rows;
    int dim2 = Amatrix.n_cols;

    std::vector<double> AiJ(dim1, 0.0);
    std::vector<double> AIj(dim2, 0.0);
    double AIJ = 0;

    for (int i = 0; i < dim1; ++i)
    {
        AiJ[i] = 0;

        for (int j = 0; j < dim2; ++j)
        {
            AiJ[i] += Amatrix[i, j];
            AIj[j] += Amatrix[i, j];
            AIJ += Amatrix[i, j];
        }

        AiJ[i] /= dim2;
    }

    AIJ /= dim1 * dim2;

    for (int j = 0; j < dim2; ++j)
    {
        AIj[j] /= dim1;
    }

    for (int i = 0; i < dim1; ++i)
    {
        for (int j = 0; j < dim2; ++j)
        {
            retVal += ((AiJ[i] * AIj[j] - Amatrix[i, j] * AIJ) * (AiJ[i] * AIj[j] - Amatrix[i, j] * AIJ)) / (AiJ[i] * AIj[j] * AiJ[i] * AIj[j]);
        }
    }

    return retVal / (dim1 * dim2);
}

double Matrix::MeanSquaredResidue(const arma::mat& Amatrix)
{
    double retVal = 0;

    int dim1 = Amatrix.n_rows;
    int dim2 = Amatrix.n_cols;

    std::vector<double> AiJ(dim1, 0.0);
    std::vector<double> AIj(dim2, 0.0);
    double AIJ = 0;

    for (int i = 0; i < dim1; ++i)
    {
        AiJ[i] = 0;

        for (int j = 0; j < dim2; ++j)
        {
            AiJ[i] += Amatrix[i, j];
            AIj[j] += Amatrix[i, j];
            AIJ += Amatrix[i, j];
        }

        AiJ[i] /= dim2;
    }

    AIJ /= dim1 * dim2;

    for (int j = 0; j < dim2; ++j)
    {
        AIj[j] /= dim1;
    }

    for (int i = 0; i < dim1; ++i)
    {
        for (int j = 0; j < dim2; ++j)
        {
            retVal += (Amatrix[i, j] - AiJ[i] - AIj[j] + AIJ) * (Amatrix[i, j] - AiJ[i] - AIj[j] + AIJ);
        }
    }

    return retVal / (dim1 * dim2);
}

double Matrix::AverageCorrelationValue(const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    double retVal = 0;

    arma::mat Amatrix = arma::zeros<arma::mat>((uint)clusterW.size(), (uint)clusterH.size());

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

    //return std::make_shared<double>(AverageSpearmansRank(Amatrix));

    retVal = AverageCorrelationValue(Amatrix);

    return retVal;
}

double Matrix::AverageCorrelationValue(const arma::mat& Amatrix)
{
    //return std::make_shared<double>(AverageSpearmansRank(Amatrix));

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
                RowValue += 0;

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
        return RowValue;
    else
        return ColumnValue;
}


double Matrix::CalculateQualityMeasure(Enums::FeatureType measure, const std::vector<int>& clusterW, const std::vector<int>& clusterH)
{
    switch(measure)
    {
        case Enums::FeatureType::ACV:
        {
            return AverageCorrelationValue(clusterW, clusterH);
        }

        case Enums::FeatureType::ASR:
        {
            return AverageSpearmansRank(clusterW, clusterH);
        }

        case Enums::FeatureType::MSR:
        {
            return MeanSquaredResidue(clusterW, clusterH);
        }

        case Enums::FeatureType::SMSR:
        {
            return ScalingMeanSquaredResidue(clusterW, clusterH);
        }

        case Enums::FeatureType::Variance:
        {
            return Variance(clusterW, clusterH);
        }

        default:
        {
            return AverageCorrelationValue(clusterW, clusterH);
        }
    }
}
