#include "common.h"

Common::Common()
{
}


bool CheckIfFileExists(QString path)
{
    QFileInfo checkFile(path);

    return (checkFile.exists() && checkFile.isFile());
}
