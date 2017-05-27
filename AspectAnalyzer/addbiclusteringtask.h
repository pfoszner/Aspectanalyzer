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
#include "biclusteringobject.h"

class FirstPage;
class SecondPage;
class ThirdPage;
class IntroductionPage;
class DataSourcePage;
class InputDataPage;

class AddBiclusteringTask : public SimpleWizard
{
    Q_OBJECT

public:
    AddBiclusteringTask(QWidget *parent = 0);
    std::vector<std::shared_ptr<BiclusteringObject>> result;

protected:
    QWidget *createPage(int index);
    void accept();

private:

    IntroductionPage *introductionPage;
    DataSourcePage *dataSourcePage;
    InputDataPage *inputDataPage;
    FirstPage *firstPage;
    SecondPage *secondPage;
    ThirdPage *thirdPage;

    friend class IntroductionPage;
    friend class InputDataPage;
    friend class InputDataSource;
    friend class FirstPage;
    friend class SecondPage;
    friend class ThirdPage;

};

class FirstPage : public QWidget
{
    Q_OBJECT

public:
    FirstPage(AddBiclusteringTask *wizard);

private slots:
    void classNameChanged();

public:
    QLabel *topLabel;
    QLabel *classNameLabel;
    QLabel *baseClassLabel;
    QLineEdit *classNameLineEdit;
    QLineEdit *baseClassLineEdit;
    QCheckBox *qobjectMacroCheckBox;
    QGroupBox *groupBox;
    QRadioButton *qobjectCtorRadioButton;
    QRadioButton *qwidgetCtorRadioButton;
    QRadioButton *defaultCtorRadioButton;
    QCheckBox *copyCtorCheckBox;

    friend class AddBiclusteringTask;
    friend class SecondPage;
    friend class ThirdPage;
};

class SecondPage : public QWidget
{
    Q_OBJECT

public:
    SecondPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;
    QCheckBox *commentCheckBox;
    QCheckBox *protectCheckBox;
    QCheckBox *includeBaseCheckBox;
    QLabel *macroNameLabel;
    QLabel *baseIncludeLabel;
    QLineEdit *macroNameLineEdit;
    QLineEdit *baseIncludeLineEdit;

    friend class AddBiclusteringTask;
};

class ThirdPage : public QWidget
{
    Q_OBJECT

public:
    ThirdPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;
    QLabel *outputDirLabel;
    QLabel *headerLabel;
    QLabel *implementationLabel;
    QLineEdit *outputDirLineEdit;
    QLineEdit *headerLineEdit;
    QLineEdit *implementationLineEdit;

    friend class AddBiclusteringTask;
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


class InputDataPage: public QWidget
{
    Q_OBJECT

public:
    InputDataPage(AddBiclusteringTask *wizard);

public:
    QLabel *topLabel;

    friend class AddBiclusteringTask;
};

#endif
