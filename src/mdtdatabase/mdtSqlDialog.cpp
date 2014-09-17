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

///#include "mdtSqlFormWidget.h"

#include "mdtSqlDataWidgetController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <memory>

#include <QDebug>

mdtSqlDialog::mdtSqlDialog(QWidget *parent)
 : QDialog(parent)
{
  pvMainLayout = new QVBoxLayout;
  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  pvForm = 0;
  createEditionElements(buttonsLayout);
  // Setup button box
  QDialogButtonBox *buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Close);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  buttonsLayout->addWidget(buttonBox);
  // Setup layout
  pvMainLayout->addLayout(buttonsLayout);
  setLayout(pvMainLayout);
}

mdtSqlDialog::~mdtSqlDialog()
{
}

void mdtSqlDialog::setSqlForm(mdtSqlForm *form)
{
  Q_ASSERT(form != 0);

  if(pvForm != 0){
    disconnectEditionElements();
  }
  pvForm = form;
  pvMainLayout->insertWidget(0, pvForm);
  connectEditionElements();
  pvForm->start();
}

void mdtSqlDialog::enableEdition()
{
  Q_ASSERT(pvForm != 0);
  ///Q_ASSERT(pvForm->mainSqlWidget() != 0);

  pbSubmit->setVisible(true);
  pbRevert->setVisible(true);
  // As default, functions are disabled
  pbSubmit->setEnabled(false);
  pbRevert->setEnabled(false);
  // Update buttons first time
  ///pvForm->mainSqlWidget()->reEnterVisualizingState();
}

void mdtSqlDialog::disableEdition()
{
  pbSubmit->setVisible(false);
  pbRevert->setVisible(false);
}

void mdtSqlDialog::accept()
{
  /// \todo Fix this !
  if((pvForm == 0) || (pvForm->allDataAreSaved())){
    QDialog::reject();
  }
}

void mdtSqlDialog::reject()
{
  /// \todo Fix this !
  if((pvForm == 0) || (pvForm->allDataAreSaved())){
    QDialog::reject();
  }
}

void mdtSqlDialog::createEditionElements(QHBoxLayout *l)
{
  Q_ASSERT(l != 0);

  // Setup save/cancel buttons
  pbSubmit = new QPushButton(tr("Save"));
  l->addWidget(pbSubmit);
  pbRevert = new QPushButton(tr("Cancel"));
  l->addWidget(pbRevert);
  // As default, edition is disabled
  pbSubmit->hide();
  pbRevert->hide();
}

void mdtSqlDialog::connectEditionElements()
{
  Q_ASSERT(pvForm != 0);
  /**
  Q_ASSERT(pvForm->mainSqlWidget() != 0);

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);
  */

  std::shared_ptr<mdtSqlDataWidgetController> controller;

  controller = pvForm->mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(controller);

  // Connect buttons enable/disable
  connect(controller.get(), SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  // Connect buttons triggers
  connect(pbSubmit, SIGNAL(clicked()), controller.get(), SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), controller.get(), SLOT(revert()));
}

void mdtSqlDialog::disconnectEditionElements()
{
  Q_ASSERT(pvForm != 0);
  /**
  Q_ASSERT(pvForm->mainSqlWidget() != 0);

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);
  */

  std::shared_ptr<mdtSqlDataWidgetController> controller;

  controller = pvForm->mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(controller);

  // Connect buttons enable/disable
  disconnect(controller.get(), SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  disconnect(controller.get(), SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  // Connect buttons triggers
  disconnect(pbSubmit, SIGNAL(clicked()), controller.get(), SLOT(submit()));
  disconnect(pbRevert, SIGNAL(clicked()), controller.get(), SLOT(revert()));
}
