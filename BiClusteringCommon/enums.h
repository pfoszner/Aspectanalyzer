#ifndef ENUMS_H
#define ENUMS_H


class Enums
{
public:
    Enums();

    enum LabelType
    {
        RowLabel = 1,
        ColumnLabel,
        RowClassLabel,
        ColumnClassLabel
    };

    enum MunkresFunc
    {
        Min = 0,
        Max
    };

    enum MatrixType
    {
        V = 1,
        W,
        H
    };

    enum Methods
        {
            PLSA = 0,
            LEAST_SQUARE_ERROR,
            KULLBACK_LIEBER,
            NonSmooth_KULLBACK_LIEBER,
            FABIA,
            FABIAS,
            QUBIC,
            CONSENSUS,
            TRICLUSTERING,
            Spectral,
            Plaid,
            OPSM,
            BBC,
            CPB,
            ISA,
            ChengandChurch,
            BiMax,
            xMOTIFs,
            COALESCE,
            PROJECT_GRADIENT_METHOD,
            Floc,
            ITL,
            kSpectral
        };

    enum MethodsParameters
    {
        MaxMunberOfSteps,
        NumberOfBiClusters,
        ExMethod,
        Multithreading,
        Theta,
        Supervised,
        SaveToLocalFile,
        SaveToDatabase,
        CutOffThreashold
    };

    enum FeatureType
    {
        SimilarityJaccard = 1,
        SimilarityLevenstine,
        MSR,
        ACV,
        ASR,
        Divergence,
        BiclusterCount,
        AverageACV,
        AverageSimilarity,
        ConsensusExtractionType,
        Variance,
        SMSR
    };

    enum SimilarityMethods
        {
            LevenshteinDistance = 0,
            JaccardIndex,
            JaccardWeighted,
            BothAsOne,
            Cluster1,
            Cluster2
        };

    enum ExtractingMethod
        {
            Average = 0,
            STD_1,
            STD_2,
            Zero,
            Avg_plus_1_STD,
            Avg_plus_2_STD,
            Avg_plus_3_STD,
            Quadrille,
            Custom,
            Automatic
        };

    enum BiclusterCompareMode
        {
            Both = 0,
            First,
            Second
        };
};

#endif // ENUMS_H
