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

class Experimental
{
public:

    std::shared_ptr<ComputingEngine> engine;

    Experimental(std::shared_ptr<ComputingEngine>& engine) : engine(engine)
    {}

    void StartCustom();
    void TestTriclustering();
    void RunConsensus();
    void RunTriclustering();
    void CompareGrandTruth();
    void CompareGrandTruthMiRNA();
    void ARFFPlay();
    std::vector <QString> GetSignature();
    void LoadKumalBiclusters();
    void RunAllConsensus();
    void RunAllTriclustering();
};

#endif // EXPERIMENTAL_H
