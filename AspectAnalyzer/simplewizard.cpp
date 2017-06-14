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

#include "simplewizard.h"

SimpleWizard::SimpleWizard(QWidget *parent)
    : QDialog(parent)
{
    cancelButton = new QPushButton(tr("Cancel"));
    backButton = new QPushButton(tr("< &Back"));
    nextButton = new QPushButton(tr("Next >"));
    finishButton = new QPushButton(tr("&Finish"));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(backButton, SIGNAL(clicked()), this, SLOT(backButtonClicked()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
    connect(finishButton, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(finishButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void SimpleWizard::setButtonEnabled(bool enable)
{
    if (history.size() == numPages)
        finishButton->setEnabled(enable);
    else
        nextButton->setEnabled(enable);
}

void SimpleWizard::setNumPages(int n)
{
    numPages = n;
    history.append(createPage(0));
    switchPage(0);
}

void SimpleWizard::backButtonClicked()
{
    nextButton->setEnabled(true);
    finishButton->setEnabled(true);

    QWidget *oldPage = history.takeLast();
    switchPage(oldPage);
    delete oldPage;
}

void SimpleWizard::nextButtonClicked()
{
    nextButton->setEnabled(true);
    finishButton->setEnabled(history.size() == numPages - 1);

    QWidget *oldPage = history.last();
    history.append(createPage(history.size()));
    switchPage(oldPage);
}

void SimpleWizard::switchPage(QWidget *oldPage)
{
    if (oldPage) {
        oldPage->hide();
        mainLayout->removeWidget(oldPage);
    }

    QWidget *newPage = history.last();
    mainLayout->insertWidget(0, newPage);
    newPage->show();
    newPage->setFocus();

    backButton->setEnabled(history.size() != 1);
    if (history.size() == numPages) {
        nextButton->setEnabled(false);
        finishButton->setDefault(true);
    } else {
        nextButton->setDefault(true);
        finishButton->setEnabled(false);
    }

    setWindowTitle(tr("%3 - Step %1 of %2")
                   .arg(history.size())
                   .arg(numPages)
                   .arg(title));
}
