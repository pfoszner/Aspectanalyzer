#include "buttonhandlers.h"


//Ctr
ButtonHandlers::ButtonHandlers(QObject *parent) :
    QObject(parent)
{
    engine = std::make_shared<ComputingEngine>();

    exper = std::make_shared<Experimental>(engine);

}

ButtonHandlers::~ButtonHandlers()
{
}

//Add new bi-clustering task to worker
void ButtonHandlers::btnAddTaskSlot()
{
    AddBiclusteringTask wizard(engine);

    wizard.exec();

    engine->setTasksLabels(QString::number(engine->GetRunning()), QString::number(engine->GetInQueue()));
}

void ButtonHandlers::btnCustomSlot(QString mode)
{
    qDebug() << "Starting custom calculation in mode = " << mode;

    exper->StartCustom(mode);

}

void ButtonHandlers::btnPlaySlot()
{
    engine->ServeQueue();
}

void ButtonHandlers::btnPauseSlot()
{

}

void ButtonHandlers::btnStopSlot()
{

}
