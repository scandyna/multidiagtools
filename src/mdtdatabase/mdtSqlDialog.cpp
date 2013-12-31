/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlDialog.h"
#include "mdtSqlForm.h"
#include "mdtSqlFormWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>

#include <QDebug>

mdtSqlDialog::mdtSqlDialog(QWidget *parent)
 : QDialog(parent)
{
  pvMainLayout = new QVBoxLayout;
  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  ///pvMainSqlWidget = 0;
  ///pvChildsTabWidget = 0;
  pvForm = 0;
  // Setup save/cancel buttons
  pbSubmit = new QPushButton(tr("Save"));
  buttonsLayout->addWidget(pbSubmit);
  pbRevert = new QPushButton(tr("Cancel"));
  buttonsLayout->addWidget(pbRevert);
  // As default, edition is disabled
  pbSubmit->hide();
  pbRevert->hide();
  // Setup button box
  QDialogButtonBox *buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Close);
  ///buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  buttonsLayout->addWidget(buttonBox);
  // Set the layout index for central widget position
  ///pvCentralWidgetIndexInMainLayout = 0;

  // Setup layout
  pvMainLayout->addLayout(buttonsLayout);
  setLayout(pvMainLayout);
}

mdtSqlDialog::~mdtSqlDialog()
{
  qDebug() << "mdtSqlDialog::~mdtSqlDialog() ...";
  qDebug() << "mdtSqlDialog::~mdtSqlDialog() DONE";
}

void mdtSqlDialog::setSqlForm(mdtSqlForm *form)
{
  Q_ASSERT(form != 0);

  pvForm = form;
  pvMainLayout->insertWidget(0, pvForm);
}

void mdtSqlDialog::enableEdition()
{
  Q_ASSERT(pvForm != 0);

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);

  pbSubmit->setVisible(true);
  pbRevert->setVisible(true);
  // As default, functions are disabled
  pbSubmit->setEnabled(false);
  pbRevert->setEnabled(false);
  // Connect buttons enable/disable
  connect(widget, SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  // Connect buttons triggers
  connect(pbSubmit, SIGNAL(clicked()), widget, SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), widget, SLOT(revert()));
}

void mdtSqlDialog::disableEdition()
{
  Q_ASSERT(pvForm != 0);

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);

}

/**
void mdtSqlDialog::setSqlWidget(mdtAbstractSqlWidget *sqlWidget)
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
*/

/**
void mdtSqlDialog::addChildWidget(QWidget *widget, const QString & label)
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
*/
/**
void mdtSqlDialog::setCurrentRow(int row)
{
  Q_ASSERT(pvMainSqlWidget != 0);

  pvMainSqlWidget->setCurrentIndex(row);
}

bool mdtSqlDialog::setCurrentRow(const QString & fieldName, const QVariant & matchData)
{
  Q_ASSERT(pvMainSqlWidget != 0);

  return pvMainSqlWidget->setCurrentRecord(fieldName, matchData);
}

bool mdtSqlDialog::setCurrentRow(const QList<QPair<QString, QVariant> > & matchList)
{
  return false;
}
*/

void mdtSqlDialog::accept()
{
  /// \todo Fix this !
  if((pvForm == 0) || (pvForm->mainSqlWidget()->allDataAreSaved())){
    QDialog::reject();
  }
}

void mdtSqlDialog::reject()
{
  /// \todo Fix this !
  if((pvForm == 0) || (pvForm->mainSqlWidget()->allDataAreSaved())){
    QDialog::reject();
  }
}
