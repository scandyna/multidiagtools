/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtSqlDialogOld.h"
#include "mdtAbstractSqlWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>

#include <QDebug>

mdtSqlDialogOld::mdtSqlDialogOld(QWidget *parent)
 : QDialog(parent)
{
  pvMainLayout = new QVBoxLayout;
  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  pvMainSqlWidget = 0;
  pvChildsTabWidget = 0;
  // Setup save/cancel buttons
  pbSubmit = new QPushButton(tr("Save"));
  buttonsLayout->addWidget(pbSubmit);
  pbRevert = new QPushButton(tr("Cancel"));
  buttonsLayout->addWidget(pbRevert);
  // Setup button box
  QDialogButtonBox *buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  buttonsLayout->addWidget(buttonBox);
  // Set the layout index for central widget position
  pvCentralWidgetIndexInMainLayout = 0;

  // Setup layout
  pvMainLayout->addLayout(buttonsLayout);
  setLayout(pvMainLayout);
}

mdtSqlDialogOld::~mdtSqlDialogOld()
{
}

void mdtSqlDialogOld::setSqlWidget(mdtAbstractSqlWidget *sqlWidget)
{
  Q_ASSERT(sqlWidget != 0);

  pvMainSqlWidget = sqlWidget;
  pvMainLayout->insertWidget(pvCentralWidgetIndexInMainLayout, pvMainSqlWidget);
  // As default, functions are disabled
  pbSubmit->setEnabled(false);
  pbRevert->setEnabled(false);
  // Connect buttons enable/disable
  connect(pvMainSqlWidget, SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  // Connect buttons triggers
  connect(pbSubmit, SIGNAL(clicked()), pvMainSqlWidget, SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), pvMainSqlWidget, SLOT(revert()));
}

void mdtSqlDialogOld::addChildWidget(QWidget *widget, const QString & label)
{
  Q_ASSERT(widget != 0);

  // Setup tab widget if needed
  if(pvChildsTabWidget == 0){
    pvChildsTabWidget = new QTabWidget;
    pvMainLayout->insertWidget(pvCentralWidgetIndexInMainLayout + 1, pvChildsTabWidget);
  }
  Q_ASSERT(pvChildsTabWidget != 0);
  pvChildsTabWidget->addTab(widget, label);
}

void mdtSqlDialogOld::setCurrentRow(int row)
{
  Q_ASSERT(pvMainSqlWidget != 0);

  pvMainSqlWidget->setCurrentIndex(row);
}

bool mdtSqlDialogOld::setCurrentRow(const QString & fieldName, const QVariant & matchData)
{
  Q_ASSERT(pvMainSqlWidget != 0);

  return pvMainSqlWidget->setCurrentRecord(fieldName, matchData);
}

bool mdtSqlDialogOld::setCurrentRow(const QList<QPair<QString, QVariant> > & matchList)
{
  return false;
}

void mdtSqlDialogOld::accept()
{
  if((pvMainSqlWidget == 0) || (pvMainSqlWidget->allDataAreSaved())){
    QDialog::reject();
  }
}

void mdtSqlDialogOld::reject()
{
  if((pvMainSqlWidget == 0) || (pvMainSqlWidget->allDataAreSaved())){
    QDialog::reject();
  }
}
