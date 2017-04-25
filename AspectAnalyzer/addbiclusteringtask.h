#ifndef ADDBICLUSTERINGTASK_H
#define ADDBICLUSTERINGTASK_H

#include <QWizard>
#include <QLabel>
#include <QVBoxLayout>

class AddBiclusteringTask : public QWizard
{
    Q_OBJECT

    public:
        AddBiclusteringTask(QWidget *parent = 0);

        void accept() override;

};



class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

private:
    QLabel *label;
};

#endif // ADDBICLUSTERINGTASK_H
