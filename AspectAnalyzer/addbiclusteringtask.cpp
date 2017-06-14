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

    AddBiclusteringTask::AddBiclusteringTask(std::shared_ptr<ComputingEngine>& engine, QWidget *parent)
        : engine(engine), SimpleWizard(parent)
    {
        setNumPages(5);
        title = "Add Bi-clustering Task Wizard";
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
            inputDataPage = new InputDataPage(this, engine);
            return inputDataPage;
        case 3:
            methodPage = new MethodPage(this);
            return methodPage;
        case 4:
            paramsPage = new ParamsPage(this);
            return paramsPage;
        }
        return 0;
    }

    void AddBiclusteringTask::accept()
    {
        //Create Bi-clustering object;
        std::shared_ptr<BiclusteringObject> newObject;

        int numOfRep = paramsPage->numRepLE->text().toInt();
        int numOfBiclusters = paramsPage->bicNumLE->text().toInt();

        for(int i = 0; i < numOfRep; ++i)
        {
            std::vector<std::tuple<Enums::MethodsParameters, std::shared_ptr<void>>> params;

            if (methodPage->plsa->isChecked())
            {
                newObject = std::make_shared<PLSA>(inputDataPage->loaddedVmatrix);
            }
            else if (methodPage->lse->isChecked())
            {
                newObject = std::make_shared<LSE>(inputDataPage->loaddedVmatrix);
            }
            else if (methodPage->kl->isChecked())
            {
                newObject = std::make_shared<KullbackLeibler>(inputDataPage->loaddedVmatrix);
            }
            else if (methodPage->nskl->isChecked())
            {
                double theta = paramsPage->thetaLE->text().toDouble();
                newObject = std::make_shared<nsKullbackLeibler>(inputDataPage->loaddedVmatrix, theta);
                params.emplace_back(Enums::Theta, std::make_shared<double>(theta));
            }


            params.emplace_back(Enums::ExMethod, std::make_shared<Enums::ExtractingMethod>(Enums::ExtractingMethod::Average));
            params.emplace_back(Enums::NumberOfBiClusters, std::make_shared<int>(numOfBiclusters));

            newObject->ParseParameters(params);

            engine->AddBiClusteringTask(newObject);
        }

        QDialog::accept();
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

    InputDataPage::InputDataPage(AddBiclusteringTask *wizard, std::shared_ptr<ComputingEngine> &engine)
        : QWidget(wizard), engine(engine), wizard(wizard)
    {
        QGridLayout *layout = new QGridLayout;

        if (wizard->dataSourcePage->dbRadioButton->isChecked())
        {
            QString intro = "<center><b>You choose SQLite database.</center><p>";

            matrixView = new QTableWidget(this);

            engine->db->GetMatrixTableList(matrixView);

            for(int i = 0; i < matrixView->rowCount(); ++i)
            {
                QPushButton* pButton = new QPushButton("Load Me", matrixView);
                matrixView->setCellWidget(i, 4, pButton);

                connect(pButton, SIGNAL(clicked()), &buttonSignalMapper, SLOT(map()));
                buttonSignalMapper.setMapping(pButton, i);
                connect(&buttonSignalMapper, SIGNAL(mapped(int)), this, SLOT(CellButtonClicked(int)));
            }

            topLabel = new QLabel(intro);

            matrixLabel = new QLabel("Loaded matrix id: no matrix loaded");

            connect(this, SIGNAL(ChangeQLabelText(QString)), matrixLabel, SLOT(setText(QString)));

            layout->addWidget(topLabel, 0, 0, 1, 2);

            layout->addWidget(matrixLabel, 1, 0);

            layout->addWidget(matrixView, 2, 0);

            layout->setRowMinimumHeight(1, 10);

            SetNextButton("id");
        }
        else
        {
            QString intro = "<center><b>You choose VMATRIX/SOFT filename.</center><p>";

            loadFile = new QPushButton("Load File", this);

            loadFilelbl = new QLabel(tr("No Matrix loaded"));

            connect(this, SIGNAL(ChangeQLabelmMain(QString)), loadFilelbl, SLOT(setText(QString)));

            connect(loadFile, SIGNAL (released()),this, SLOT (handleloadFileButton()));

            topLabel = new QLabel(intro);

            matrixLabel = new QLabel("Loaded matrix filename: no matrix loaded");

            connect(this, SIGNAL(ChangeQLabelText(QString)), matrixLabel, SLOT(setText(QString)));

            matrixSize = new QLabel("Loaded matrix size: no matrix loaded");

            connect(this, SIGNAL(ChangeQLabelmSize(QString)), matrixSize, SLOT(setText(QString)));

            layout->addWidget(topLabel, 0, 0, 1, 2);
            layout->addWidget(loadFile, 1, 0);
            layout->addWidget(loadFilelbl, 1, 1);
            layout->addWidget(matrixLabel, 2, 0, 1, 2);
            layout->addWidget(matrixSize, 3, 0, 1, 2);
            layout->setRowMinimumHeight(1, 10);

            SetNextButton("filename");
        }

        layout->setRowStretch(5, 1);
        setLayout(layout);
    }

    void InputDataPage::handleloadFileButton()
    {
        QString fileName = QFileDialog::getOpenFileName(nullptr,
            tr("Open Image"), "", tr("AspectAnalyzer Files (*.vmatrix *.soft)"));

        emit ChangeQLabelmMain("Matrix is loading please wait");

        if (fileName.isEmpty() || fileName.isNull())
        {
            if (loaddedVmatrix == nullptr)
            {
                emit ChangeQLabelText("Loaded matrix filename: no matrix loaded");

                emit ChangeQLabelmSize("Loaded matrix size: no matrix loaded");

                emit ChangeQLabelmMain("No Matrix loaded");

                wizard->setButtonEnabled(false);
            }
        }
        else
        {
            QFileInfo fi(fileName);

            emit ChangeQLabelText("Loaded matrix filename: " + fi.baseName());

            loaddedVmatrix = std::make_shared<Matrix>(fileName);

            emit ChangeQLabelmSize("Loaded matrix size: " + QString::number(loaddedVmatrix->data.n_rows) + "x" + QString::number(loaddedVmatrix->data.n_cols));

            emit ChangeQLabelmMain("Matrix loaded sucessfully");

            wizard->setButtonEnabled(true);
        }
    }

    void InputDataPage::SetNextButton(QString mode)
    {
        if (loaddedVmatrix == nullptr)
        {
            emit ChangeQLabelText("Loaded matrix " + mode + ": no matrix loaded");

            wizard->setButtonEnabled(false);
        }
        else
        {
            emit ChangeQLabelText("Loaded matrix " + mode + ": " + QString::number(*loaddedVmatrix->idMatrix));

            wizard->setButtonEnabled(true);
        }
    }


    void InputDataPage::CellButtonClicked(int rowNum)
    {
        emit ChangeQLabelText("Loaded matrix id: please wait while matrix is loading ...");
        int idMatrix = matrixView->itemAt(rowNum, 0)->text().toInt();
        loaddedVmatrix = engine->db->GetMatrix(idMatrix);
        SetNextButton("id");
    }

    MethodPage::MethodPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        QString intro = "<center><b>Method selection</center><p>";

        topLabel = new QLabel(intro);

        groupBox = new QGroupBox(tr("Choose bi-clustering methods:"));

        plsa = new QRadioButton(tr("PLSA"));
        lse = new QRadioButton(tr("Least Square Error"));
        kl = new QRadioButton(tr("Kullback-Liebler"));
        nskl = new QRadioButton(tr("non-smooth Kullback-Liebler"));

        plsa->setChecked(true);

        QVBoxLayout *groupBoxLayout = new QVBoxLayout;
        groupBoxLayout->addWidget(plsa);
        groupBoxLayout->addWidget(lse);
        groupBoxLayout->addWidget(kl);
        groupBoxLayout->addWidget(nskl);
        groupBox->setLayout(groupBoxLayout);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel);
        layout->setRowMinimumHeight(1, 10);
        layout->addWidget(groupBox);
        layout->setRowStretch(5, 1);
        setLayout(layout);
    }

    ParamsPage::ParamsPage(AddBiclusteringTask *wizard)
        : QWidget(wizard)
    {
        QString methodS = "PLSA";

        if (wizard->methodPage->lse->isChecked())
        {
            methodS = "Least Square Error";
        }
        else if (wizard->methodPage->kl->isChecked())
        {
            methodS = "Kullback-Liebler";
        }
        else if (wizard->methodPage->nskl->isChecked())
        {
            methodS = "non-smooth Kullback-Liebler";
        }

        QString intro = "<center><b>Parameters tunning method: " + methodS + "</center><p>";

        topLabel = new QLabel(intro);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(topLabel);

        int rowNum = 1;

        QLocale locale;

        QString separator = locale.decimalPoint();

//number of bi-clusters
        if (    wizard->methodPage->plsa->isChecked()
             || wizard->methodPage->lse->isChecked()
             || wizard->methodPage->kl->isChecked()
             || wizard->methodPage->nskl->isChecked()
           )
        {
            bicNum = new QLabel("Number of bi-clusters: <1 - 1024>");
            layout->addWidget(bicNum, rowNum, 0);
            bicNumLE = new QLineEdit;
            bicNumLE->setText("8");
            bicNumLE->setValidator(new QIntValidator(1,1024,bicNumLE));
            layout->addWidget(bicNumLE, rowNum, 1);
            rowNum++;

            extrction = new QLabel("Extraction Type: ");
            extrctionCB = new QComboBox;
            extrctionCB->setEditable(false);
            extrctionCB->addItem("Zero");
            extrctionCB->addItem("Average");
            extrctionCB->addItem("Quartile");
            layout->addWidget(extrction, rowNum, 0);
            layout->addWidget(extrctionCB, rowNum, 1);
            rowNum++;

            if (wizard->methodPage->nskl->isChecked())
            {
                theta = new QLabel("Theta: <0 - 1>");
                layout->addWidget(theta, rowNum, 0);
                thetaLE = new QLineEdit;

                thetaLE->setText("0" + separator + "5");
                thetaLE->setValidator(new QDoubleValidator(0.0,1.0,5,thetaLE));
                layout->addWidget(thetaLE, rowNum, 1);
                rowNum++;
            }

            numRep = new QLabel("Number of Repetition: <1 - 1000000>");
            layout->addWidget(numRep, rowNum, 0);
            numRepLE = new QLineEdit;
            numRepLE->setText("10");
            numRepLE->setValidator(new QIntValidator(1,1000000,numRepLE));
            layout->addWidget(numRepLE, rowNum, 1);
            rowNum++;
        }

//Extracion Type
//Repetition


        //layout->setRowStretch(5, 1);
        setLayout(layout);
    }
