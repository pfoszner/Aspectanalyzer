#ifndef EXPERIMENTAL_H
#define EXPERIMENTAL_H

#include "matrix.h"
#include "common.h"
#include "dbtools.h"
#include "singlethreadworker.h"
#include "computingengine.h"
#include "plsa.h"
#include "lse.h"
#include "kullbackleibler.h"
#include "nskullbackleibler.h"
#include "consensus.h"
#include "triclustering.h"
#include "ensembletask.h"

class Experimental
{
public:

    std::shared_ptr<ComputingEngine> engine;

    Experimental(std::shared_ptr<ComputingEngine>& engine) : engine(engine)
    {}

    void StartCustom(QString mode);
    void TestTriclustering();
    void RunConsensus(int method, std::vector<Enums::Methods> methods, QString desc);
    void RunTriclustering(int matrix);
    void CompareGrandTruth();
    void CompareGrandTruthMiRNA();
    void ARFFPlay();
    std::vector <QString> GetSignature();
    void LoadKumalBiclusters();
    void RunAllConsensus();
    void RunAllConsensus2(int idMeter);
    void RunAllTriclustering();
    void Squro(QString mode);
    void RunNMF(int matrix, int start, int stop, int step, uint rep);
    void RunStepConsensus(int matrix, int start, int stop, int step);
    void RunStepTricluster(int matrix, int start);
    void Muszki();
    void ExportResults(QString folder, int startId, int stopID);
    void PLATResults(QString folder, int startId, int stopID, int classNum);
    void InputForBingo(QString file, std::vector<int> startID, int endID);
    void CheckSimiliarity();
    void DividePowerlogs();
    void ImportKumalResults(int repNum);
    void ImportKumalResultsNoise(int repNum);
    void ImportKumalResultsNumber(int repNum);
    void ImportKumalResultsOverlap(int repNum);
    void PringInfo(int taskID, QString filename);
};

#endif // EXPERIMENTAL_H
