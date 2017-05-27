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

    #include <QtGui>

    #include "addbiclusteringtask.h"

    AddBiclusteringTask::AddBiclusteringTask(QWidget *parent)
        : SimpleWizard(parent)
    {
        setNumPages(3);
    }

    QWidget *AddBiclusteringTask::createPage(int index)
    {
        switch (index) {
        case 0:
            introductionPage = new IntroductionPage(this);
            return introductionPage;
        case 1:
            dataSourcePage = new DataSourcePage(this);
            return dataSourcePage;
        case 2:
            inputDataPage = new InputDataPage(this);
            return inputDataPage;
        //case 2:
        //    firstPage = new FirstPage(this);
        //    return firstPage;
        //case 3:
        //    secondPage = new SecondPage(this);
        //    return secondPage;
        //case 4:
        //    thirdPage = new ThirdPage(this);
        //    return thirdPage;
        }
        return 0;
    }

    void AddBiclusteringTask::accept()
    {
        //Create Bi-clustering object;

        QDialog::accept();
    }

    FirstPage::FirstPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        topLabel = new QLabel(tr("<center><b>Class information</b></center>"
                                 "<p>This wizard will generate a skeleton class "
                                 "definition and member function definitions."));
        topLabel->setWordWrap(false);

        classNameLabel = new QLabel(tr("Class &name:"));
        classNameLineEdit = new QLineEdit;
        classNameLabel->setBuddy(classNameLineEdit);
        setFocusProxy(classNameLineEdit);

        baseClassLabel = new QLabel(tr("&Base class:"));
        baseClassLineEdit = new QLineEdit;
        baseClassLabel->setBuddy(baseClassLineEdit);

        qobjectMacroCheckBox = new QCheckBox(tr("&Generate Q_OBJECT macro"));

        groupBox = new QGroupBox(tr("&Constructor"));

        qobjectCtorRadioButton = new QRadioButton(tr("&QObject-style constructor"));
        qwidgetCtorRadioButton = new QRadioButton(tr("Q&Widget-style constructor"));
        defaultCtorRadioButton = new QRadioButton(tr("&Default constructor"));
        copyCtorCheckBox = new QCheckBox(tr("&Also generate copy constructor and "
                                            "assignment operator"));

        defaultCtorRadioButton->setChecked(true);

        connect(classNameLineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(classNameChanged()));
        connect(defaultCtorRadioButton, SIGNAL(toggled(bool)),
                copyCtorCheckBox, SLOT(setEnabled(bool)));

        wizard->setButtonEnabled(false);

        QVBoxLayout *groupBoxLayout = new QVBoxLayout;
        groupBoxLayout->addWidget(qobjectCtorRadioButton);
        groupBoxLayout->addWidget(qwidgetCtorRadioButton);
        groupBoxLayout->addWidget(defaultCtorRadioButton);
        groupBoxLayout->addWidget(copyCtorCheckBox);
        groupBox->setLayout(groupBoxLayout);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel, 0, 0, 1, 2);
        layout->setRowMinimumHeight(1, 10);
        layout->addWidget(classNameLabel, 2, 0);
        layout->addWidget(classNameLineEdit, 2, 1);
        layout->addWidget(baseClassLabel, 3, 0);
        layout->addWidget(baseClassLineEdit, 3, 1);
        layout->addWidget(qobjectMacroCheckBox, 4, 0, 1, 2);
        layout->addWidget(groupBox, 5, 0, 1, 2);
        layout->setRowStretch(6, 1);
        setLayout(layout);
    }

    void FirstPage::classNameChanged()
    {
        AddBiclusteringTask *wizard = qobject_cast<AddBiclusteringTask *>(parent());
        wizard->setButtonEnabled(!classNameLineEdit->text().isEmpty());
    }

    SecondPage::SecondPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        topLabel = new QLabel(tr("<center><b>Code style options</b></center>"));

        commentCheckBox = new QCheckBox(tr("&Start generated files with a comment"));
        commentCheckBox->setChecked(true);
        setFocusProxy(commentCheckBox);

        protectCheckBox = new QCheckBox(tr("&Protect header file against multiple "
                                           "inclusions"));
        protectCheckBox->setChecked(true);

        macroNameLabel = new QLabel(tr("&Macro name:"));
        macroNameLineEdit = new QLineEdit;
        macroNameLabel->setBuddy(macroNameLineEdit);

        includeBaseCheckBox = new QCheckBox(tr("&Include base class definition"));
        baseIncludeLabel = new QLabel(tr("Base class include:"));
        baseIncludeLineEdit = new QLineEdit;
        baseIncludeLabel->setBuddy(baseIncludeLineEdit);

        QString className = wizard->firstPage->classNameLineEdit->text();
        macroNameLineEdit->setText(className.toUpper() + "_H");

        QString baseClass = wizard->firstPage->baseClassLineEdit->text();
        if (baseClass.isEmpty()) {
            includeBaseCheckBox->setEnabled(false);
            baseIncludeLabel->setEnabled(false);
            baseIncludeLineEdit->setEnabled(false);
        } else {
            includeBaseCheckBox->setChecked(true);
            if (QRegExp("Q[A-Z].*").exactMatch(baseClass)) {
                baseIncludeLineEdit->setText("<" + baseClass + ">");
            } else {
                baseIncludeLineEdit->setText("\"" + baseClass.toLower() + ".h\"");
            }
        }

        connect(protectCheckBox, SIGNAL(toggled(bool)),
                macroNameLabel, SLOT(setEnabled(bool)));
        connect(protectCheckBox, SIGNAL(toggled(bool)),
                macroNameLineEdit, SLOT(setEnabled(bool)));
        connect(includeBaseCheckBox, SIGNAL(toggled(bool)),
                baseIncludeLabel, SLOT(setEnabled(bool)));
        connect(includeBaseCheckBox, SIGNAL(toggled(bool)),
                baseIncludeLineEdit, SLOT(setEnabled(bool)));

        QGridLayout *layout = new QGridLayout;
        layout->setColumnMinimumWidth(0, 20);
        layout->addWidget(topLabel, 0, 0, 1, 3);
        layout->setRowMinimumHeight(1, 10);
        layout->addWidget(commentCheckBox, 2, 0, 1, 3);
        layout->addWidget(protectCheckBox, 3, 0, 1, 3);
        layout->addWidget(macroNameLabel, 4, 1);
        layout->addWidget(macroNameLineEdit, 4, 2);
        layout->addWidget(includeBaseCheckBox, 5, 0, 1, 3);
        layout->addWidget(baseIncludeLabel, 6, 1);
        layout->addWidget(baseIncludeLineEdit, 6, 2);
        layout->setRowStretch(7, 1);
        setLayout(layout);
    }

    ThirdPage::ThirdPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        topLabel = new QLabel(tr("<center><b>Output files</b></center>"));

        outputDirLabel = new QLabel(tr("&Output directory:"));
        outputDirLineEdit = new QLineEdit;
        outputDirLabel->setBuddy(outputDirLineEdit);
        setFocusProxy(outputDirLineEdit);

        headerLabel = new QLabel(tr("&Header file name:"));
        headerLineEdit = new QLineEdit;
        headerLabel->setBuddy(headerLineEdit);

        implementationLabel = new QLabel(tr("&Implementation file name:"));
        implementationLineEdit = new QLineEdit;
        implementationLabel->setBuddy(implementationLineEdit);

        QString className = wizard->firstPage->classNameLineEdit->text();
        headerLineEdit->setText(className.toLower() + ".h");
        implementationLineEdit->setText(className.toLower() + ".cpp");
        outputDirLineEdit->setText(QDir::toNativeSeparators(QDir::homePath()));

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel, 0, 0, 1, 2);
        layout->setRowMinimumHeight(1, 10);
        layout->addWidget(outputDirLabel, 2, 0);
        layout->addWidget(outputDirLineEdit, 2, 1);
        layout->addWidget(headerLabel, 3, 0);
        layout->addWidget(headerLineEdit, 3, 1);
        layout->addWidget(implementationLabel, 4, 0);
        layout->addWidget(implementationLineEdit, 4, 1);
        layout->setRowStretch(5, 1);
        setLayout(layout);
    }

    IntroductionPage::IntroductionPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        QString intro = "<center><b>This wizard will help You create bi-clustering task.</center><p>";

        intro += "You will be led by a process that consists of the following stages: <p>";

        intro += "<ul>";
        intro += "<li>Choosing the input matrix</li>";
        intro += "<li>Selection of the bi-clustering algorithm</li>";
        intro += "<li>Tuning the parameters of the selected algorithm</li>";
        intro += "<li>Choosing how to save results</li>";
        intro += "</ul><p>";

        intro += "Any comments, suggestions or bugs reporting please send to <a href:\"mailto:aspectanalyzer@foszner.pl\">aspectanalyzer@foszner.pl</a>";

        intro += "</b>";

        topLabel = new QLabel(intro);

        topLabel->setOpenExternalLinks(true);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel, 0, 0, 1, 2);
        layout->setRowMinimumHeight(1, 10);
        layout->setRowStretch(5, 1);
        setLayout(layout);
    }


    DataSourcePage::DataSourcePage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        QString intro = "<center><b>Select The source of Your input data matrix?</center><p>";

        topLabel = new QLabel(intro);

        groupBox = new QGroupBox(tr("Choose Your data source"));

        fileRadioButton = new QRadioButton(tr("VMATRIX/ARFF file"));
        dbRadioButton = new QRadioButton(tr("Database"));

        fileRadioButton->setChecked(true);

        QVBoxLayout *groupBoxLayout = new QVBoxLayout;
        groupBoxLayout->addWidget(dbRadioButton);
        groupBoxLayout->addWidget(fileRadioButton);
        groupBox->setLayout(groupBoxLayout);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel);
        layout->setRowMinimumHeight(1, 10);
        layout->addWidget(groupBox);
        layout->setRowStretch(5, 1);
        setLayout(layout);
    }

    InputDataPage::InputDataPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        if (wizard->dataSourcePage->dbRadioButton->isChecked())
        {
            QString intro = "<center><b>You choose SQLite database.</center><p>";

            topLabel = new QLabel(intro);
        }
        else
        {
            QString intro = "<center><b>You choose VMATRIX filename.</center><p>";

            topLabel = new QLabel(intro);
        }


        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel, 0, 0, 1, 2);
        layout->setRowMinimumHeight(1, 10);
        layout->setRowStretch(5, 1);
        setLayout(layout);
    }
