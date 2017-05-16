#ifndef LABEL_H
#define LABEL_H

#include <QString>

class Label
{

public:
    Label(int idLabel, int idMatrix, int idLabelType, int indexNbr, QString value) : idLabel(idLabel), idMatrix(idMatrix), idLabelType(idLabelType), indexNbr(indexNbr), value(value)
    { }

    int idLabel;
    int idMatrix;
    int idLabelType;
    int indexNbr;
    QString value;

};

#endif // LABEL_H
