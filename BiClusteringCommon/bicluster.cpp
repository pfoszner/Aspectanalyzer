#include "bicluster.h"


Bicluster::Bicluster(const Bicluster& Copy)
{
    idBicluster = Copy.idBicluster;

    cluster1.clear();
    cluster2.clear();

    std::copy(Copy.cluster1.begin(), Copy.cluster1.end(), cluster1.begin());

    std::copy(Copy.cluster2.begin(), Copy.cluster2.end(), cluster2.begin());

    std::copy(Copy.mesures.begin(), Copy.mesures.end(), mesures.begin());
}

Bicluster::Bicluster(int IdBicluster, const std::vector<int>& cluster1, const std::vector<int>& cluster2)
{
    this->idBicluster = IdBicluster;

    this->cluster1.clear();
    this->cluster2.clear();

    //Empt mode to balance Munkres cost matrix
    if (cluster1.size() == 0 && cluster2.size() == 0)
    {

    }
    else
    {
        if (cluster1.size() == 0 || cluster2.size() == 0)
        {

        }
        else
        {
            this->cluster1.insert(this->cluster1.end(), cluster1.begin(), cluster1.end());
            this->cluster2.insert(this->cluster2.end(), cluster2.begin(), cluster2.end());
        }
    }
}

Bicluster::Bicluster(int X, int Y, int len1, int len2)
{
    cluster1 = std::vector<int>();
    cluster2 = std::vector<int>();

    for (int i = X; i < X + len1; ++i)
    {
        for (int j = Y; j < Y + len2; ++j)
        {
            if (std::find(cluster1.begin(), cluster1.end(), i) == cluster1.end())
            {
                cluster1.push_back(i);
            }
            if (std::find(cluster2.begin(), cluster2.end(), j) == cluster2.end())
            {
                cluster2.push_back(j);
            }
        }
    }
}

std::shared_ptr<double> Bicluster::GetFeature(Enums::FeatureType type)
{
    lock.lock();
    std::vector<FeatureResult>::iterator iter = std::find_if(mesures.begin(), mesures.end(), [type](FeatureResult q) { return q.type == type; } );

    if (iter == mesures.end())
    {
        lock.unlock();
        return nullptr;
    }
    else
    {
        std::shared_ptr<double> retVal = std::make_shared<double>(iter->value);
        lock.unlock();
        return retVal;
    }

}

void Bicluster::SetFeature(Enums::FeatureType type, double value)
{
    lock.lock();

    auto riter = std::remove_if(mesures.begin(), mesures.end(), [type](FeatureResult r) { return r.type == type; });
    mesures.erase(riter, mesures.end());

    FeatureResult newFeature(type, value, 0);

    mesures.push_back(newFeature);

    lock.unlock();
}

void Bicluster::SetFeature(FeatureResult newFeature)
{
    lock.lock();
    mesures.push_back(newFeature);
    lock.unlock();
}

double Bicluster::Compare(std::shared_ptr<Bicluster> CompareTo, Enums::SimilarityMethods type)
{
    switch (type)
    {
        case Enums::SimilarityMethods::LevenshteinDistance:
        {
            return (LevenshteinDistance(CompareTo->cluster1, cluster1) + LevenshteinDistance(CompareTo->cluster2, cluster2)) / 2;
        }
        case Enums::SimilarityMethods::JaccardIndex:
        {
            return (JaccardIndex(CompareTo->cluster1, cluster1) + JaccardIndex(CompareTo->cluster2, cluster2)) / 2;
        }
        case Enums::SimilarityMethods::JaccardWeighted:
        {
            if (CompareTo->cluster1.size() + CompareTo->cluster2.size() + cluster1.size() + cluster2.size() > 0)
                return (((CompareTo->cluster1.size() + cluster1.size()) * JaccardIndex(CompareTo->cluster1, cluster1)) + ((CompareTo->cluster2.size() + cluster2.size()) * JaccardIndex(CompareTo->cluster2, cluster2))) / (CompareTo->cluster1.size() + CompareTo->cluster2.size() + cluster1.size() + cluster2.size());
            else
                return 0;
        }
        case Enums::SimilarityMethods::Cluster1:
        {
            return JaccardIndex(CompareTo->cluster1, cluster1);
        }
        case Enums::SimilarityMethods::Cluster2:
        {
            return JaccardIndex(CompareTo->cluster2, cluster2);
        }
        case Enums::SimilarityMethods::BothAsOne:
        {
            std::vector<QString> first;
            std::vector<QString> second;

            for(int item : cluster2)
            {
                first.push_back("H{0}" + QString::number(item));
            }

            for(int item : cluster1)
            {
                first.push_back("W{0}" + QString::number(item));
            }

            for (int item : CompareTo->cluster2)
            {
                second.push_back("H{0}" + QString::number(item));
            }

            for (int item : CompareTo->cluster1)
            {
                second.push_back("W{0}" + QString::number(item));
            }

            return JaccardIndex(first, second);
        }
        default:
        {
            return (JaccardIndex(CompareTo->cluster1, cluster1) + JaccardIndex(CompareTo->cluster2, cluster2)) / 2;
        }
    }
}

double Bicluster::CompareFirst(std::shared_ptr<Bicluster> CompareTo, Enums::SimilarityMethods type)
{
    switch (type)
    {
        case Enums::SimilarityMethods::LevenshteinDistance:
        {
            return LevenshteinDistance(CompareTo->cluster1, cluster1);
        }
    case Enums::SimilarityMethods::JaccardIndex:
        {
            return JaccardIndex(CompareTo->cluster1, cluster1);
        }
        default:
        {
            return JaccardIndex(CompareTo->cluster1, cluster1);
        }
    }
}

double Bicluster::CompareSecond(std::shared_ptr<Bicluster> CompareTo, Enums::SimilarityMethods type)
{
    switch (type)
    {
        case Enums::SimilarityMethods::LevenshteinDistance:
        {
            return LevenshteinDistance(CompareTo->cluster2, cluster2);
        }
        case Enums::SimilarityMethods::JaccardIndex:
        {
            return JaccardIndex(CompareTo->cluster2, cluster2);
        }
        default:
        {
            return JaccardIndex(CompareTo->cluster2, cluster2);
        }
    }
}

double Bicluster::JaccardIndex(const std::vector<QString>& s, const std::vector<QString>& t)
{
    double RetVal = 0;

    std::vector<QString> sum;
    std::vector<QString> inter;

    for (QString item : s)
    {
        sum.push_back(item);
    }

    for (QString item : t)
    {
        if (std::find(sum.begin(), sum.end(), item) != sum.end())
        {
            inter.push_back(item);
        }
        else
        {
            sum.push_back(item);
        }
    }

    if (sum.size() == 0)
        return 0.0;

    try
    {
        RetVal = (double)inter.size() / (double)sum.size();
    }
    catch(...)
    {
        RetVal = 0;
    }

    return RetVal;
}

double Bicluster::JaccardIndex(const std::vector<int>& s, const std::vector<int>& t)
{
    double RetVal = 0;

    std::vector<int> sum;
    std::vector<int> inter;

    for (int item : s)
    {
        sum.push_back(item);
    }

    for (int item : t)
    {
        if (std::find(sum.begin(), sum.end(), item) != sum.end())
        {
            inter.push_back(item);
        }
        else
        {
            sum.push_back(item);
        }
    }

    if (sum.size() == 0)
        return 0.0;

    try
    {
        RetVal = (double)inter.size() / (double)sum.size();
    }
    catch(...)
    {
        RetVal = 0;
    }

    return RetVal;
}

uint Bicluster::LevenshteinDistance(const std::vector<int>& s, const std::vector<int>& t)
{
    uint n = (uint)s.size();
    uint m = (uint)t.size();
    Array<uint> d(n + 1, m + 1);

    // Step 1
    if (n == 0) { return m; }

    if (m == 0) { return n; }

    // Step 2
    for (uint i = 0; i <= n; i++) { d[i][0] = i; }

    for (uint j = 0; j <= m; j++) { d[0][j] = j; }

    // Step 3
    for (uint i = 1; i <= n; i++)
    {
        //Step 4
        for (uint j = 1; j <= m; j++)
        {
            // Step 5
            int cost = (t[j - 1] == s[i - 1]) ? 0 : 1;

            // Step 6
            d[i][j] = std::min(
            std::min(d[i - 1][j] + 1, d[i][j - 1] + 1),
            d[i - 1][j - 1] + cost);
        }
    }

    // Step 7
    return d[n][m];
}
