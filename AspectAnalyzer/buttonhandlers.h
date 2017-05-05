#ifndef BUTTONHANDLERS_H
#define BUTTONHANDLERS_H

#include <QObject>
#include <QDebug>
#include "computingengine.h"
#include "plsa.h"
#include "lse.h"
#include "kullbackleibler.h"
#include "nskullbackleibler.h"
#include <QFileDialog>
#include "consensus.h"
#include "triclustering.h"
#include "experimental.h"
#include "addbiclusteringtask.h"
#include "client.h"

class ButtonHandlers : public QObject
{
    Q_OBJECT
public:
    explicit ButtonHandlers(QObject *parent = 0);
    virtual ~ButtonHandlers();
    void Test();
    static ButtonHandlers btnHandler;
    std::shared_ptr<ComputingEngine> engine;
    std::shared_ptr<Experimental> exper;


public slots:
    void btnAddTaskSlot(int);
    //void btnLoadFromFileSlot();
    //void btnLoadFromDatabaseSlot();
    //void btnTuneSlot();
    void btnPlaySlot();
    void btnPauseSlot();
    void btnStopSlot();
    void btnCustomSlot(QString);


signals:
    void setMatrixLabels(const QString &text, const QString &nr, const QString &nc, const QString &min, const QString &max, const QString &dbID);
    void setTasksLabels(const QString &running, const QString &inqueue);

};

#endif // BUTTONHANDLERS_H
