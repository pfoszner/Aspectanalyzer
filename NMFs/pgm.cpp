#include "pgm.h"

PGM::PGM(std::shared_ptr<Matrix>& matrixData) : NMF(matrixData)
{
    idMethod = Enums::PROJECT_GRADIENT_METHOD;
}

void PGM::UpdateStep()
{
    //[W,gradW,iterW] = nlssubprob(V’,H’,W’,tolW,1000); W = W’; gradW = gradW’;
    //if iterW==1,
    //tolW = 0.1 * tolW;
    //end
    //[H,gradH,iterH] = nlssubprob(V,W,H,tolH,1000);
    //if iterH==1,
    //tolH = 0.1 * tolH;
    //end
}

double PGM::DivernegceValue()
{
    //double projnorm = norm([gradW(gradW<0 | W>0); gradH(gradH<0 | H>0)]);
    //if (projnorm < tol*initgrad)
        return 0.0;
    //else
        //return projnorm;
}

void PGM::PreProcessing()
{
    gradW = WMatrix * (HMatrix * HMatrix.t()) - dataMatrix->data * HMatrix.t();
    gradH = (WMatrix.t() * WMatrix) * HMatrix - WMatrix.t() * dataMatrix->data;
    /*[gradW; gradH’]*/
    //initgrad = arma::norm(arma::join_vert(gradW,gradH.t()) ,"fro");
    //tolW = arma::max(0.001, tol) * initgrad;
    //tolW = tolH;
}
