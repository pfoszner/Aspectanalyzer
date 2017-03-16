#include "costmatrixworker.h"

void CostMatrixWorker::run()
{

        switch (mode)
        {
            case Enums::BiclusterCompareMode::Both:
                *cm = bic1->Compare(bic2, simMethod);
                break;
            case Enums::BiclusterCompareMode::First:
                *cm = bic1->CompareFirst(bic2, simMethod);
                break;
            case Enums::BiclusterCompareMode::Second:
                *cm = bic1->CompareSecond(bic2, simMethod);
                break;
        }
}
