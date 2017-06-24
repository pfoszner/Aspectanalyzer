#ifndef ENSEMBLETASK_H
#define ENSEMBLETASK_H

#include "biclusteringobject.h"

enum class MergeType
    {
        Standard = 0,
        ByACV,
        ByACVHeuristic
    };

class EnsembleTask : public BiclusteringObject, public std::enable_shared_from_this<EnsembleTask>
{
public:

    EnsembleTask(std::shared_ptr<Matrix>& Vmatrix, Enums::Methods Method, int IdResult)
        : BiclusteringObject(Vmatrix, Method, IdResult, -1.0)
    {

    }

    void SetEnsemble(const std::vector<std::shared_ptr<BiclusteringObject>>& Ensemble);

    void ParseParameters(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>&);

    std::vector<std::vector<std::shared_ptr<Bicluster>>> GetBiclustersListGruppedByTask();

    std::vector<std::shared_ptr<Bicluster>> GetBiclustersList();

    void AddTaskToEnsemble(std::shared_ptr<BiclusteringObject> task);

    virtual std::shared_ptr<BiclusteringObject> Compute(std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>>& params);

protected:
    std::vector<std::shared_ptr<BiclusteringObject>> tasksToEnsemble;
    bool multithreading = true;
};

#endif // ENSEMBLETASK_H
