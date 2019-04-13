/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef CLASSWIZARD_H
#define CLASSWIZARD_H

#include "simplewizard.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QTableWidget>
#include "biclusteringobject.h"
#include "computingengine.h"
#include <QFileDialog>
#include <QComboBox>
#include <QLocale>
#include "computingengine.h"
#include "plsa.h"
#include "lse.h"
#include "kullbackleibler.h"
#include "nskullbackleibler.h"


class IntroductionPage;
class DataSourcePage;
class InputDataPage;
class MethodPage;
class ParamsPage;
class ResultsPage;

class AddBiclusteringTask : public SimpleWizard
{
    Q_OBJECT

public:
    AddBiclusteringTask(std::shared_ptr<ComputingEngine>& engine, QWidget *parent = 0);
    std::vector<std::shared_ptr<BiclusteringObject>> result;

protected:
    QWidget *createPage(int index);
    std::shared_ptr<ComputingEngine> engine;
    void accept();

private:

    IntroductionPage *introductionPage;
    DataSourcePage *dataSourcePage;
    InputDataPage *inputDataPage;
    MethodPage *methodPage;
    ParamsPage *paramsPage;
    ResultsPage *resultsPage;

    friend class IntroductionPage;
    friend class InputDataPage;
    friend class InputDataSource;
    friend class MethodPage;
    friend class ParamsPage;
    friend class ResultsPage;

};

class IntroductionPage : public QWidget
{
    Q_OBJECT

public:
    IntroductionPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;

    friend class AddBiclusteringTask;
};

class DataSourcePage : public QWidget
{
    Q_OBJECT

public:
    DataSourcePage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;
    QGroupBox *groupBox;
    QRadioButton *fileRadioButton;
    QRadioButton *dbRadioButton;

    friend class AddBiclusteringTask;
};


class InputDataPage : public QWidget
{
    Q_OBJECT

public:
    InputDataPage(AddBiclusteringTask *wizard, std::shared_ptr<ComputingEngine>& engine);
    void SetNextButton(QString mode);

public:
    std::shared_ptr<ComputingEngine> engine;
    QLabel *topLabel;
    QLabel *matrixLabel;
    QTableWidget *matrixView;
    QSignalMapper buttonSignalMapper;
    std::shared_ptr<Matrix> loaddedVmatrix;
    AddBiclusteringTask *wizard;
    QPushButton *loadFile;
    QLabel *loadFilelbl;
    QLabel *matrixSize;

signals:
    void ChangeQLabelText(QString text);
    void ChangeQLabelmSize(QString text);
    void ChangeQLabelmMain(QString text);

public slots:
    void CellButtonClicked(int rowNum);
    void handleloadFileButton();

    friend class AddBiclusteringTask;
};

class MethodPage : public QWidget
{
    Q_OBJECT

public:
    MethodPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;
    QGroupBox *groupBox;
    QRadioButton *plsa;
    QRadioButton *lse;
    QRadioButton *kl;
    QRadioButton *nskl;

    friend class AddBiclusteringTask;
};

class ParamsPage : public QWidget
{
    Q_OBJECT

public:
    ParamsPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;
    QLabel *bicNum;
    QLineEdit *bicNumLE;
    QLabel *numRep;
    QLineEdit *numRepLE;
    QLabel *extrction;
    QComboBox *extrctionCB;
    QLabel *theta;
    QLineEdit *thetaLE;
    QLabel *threashold;
    QLineEdit *threasholdLE;

    friend class AddBiclusteringTask;
};

class ResultsPage : public QWidget
{
    Q_OBJECT

public:
    ResultsPage(AddBiclusteringTask *wizard);

signals:
    void ChangeQLabelDir(QString text);

public slots:
    void handleloadFileButton();

public:
    QLabel *topLabel;
    QLabel *saveFileLabel;
    QCheckBox *saveToFileBox;
    QCheckBox *saveToDBBox;
    QPushButton *chooseDirButton;
    QFileDialog *chooseDir;

    friend class AddBiclusteringTask;
};

#endif
