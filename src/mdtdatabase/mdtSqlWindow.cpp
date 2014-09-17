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
#include "mdtSqlWindow.h"

#include "mdtSqlForm.h"
///#include "mdtAbstractSqlWidget.h"
///#include "mdtSqlFormWidget.h"

#include "mdtSqlDataWidgetController.h"
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QKeySequence>
#include <QVBoxLayout>
#include <QIcon>

#include <QDebug>

mdtSqlWindow::mdtSqlWindow(QWidget *parent, Qt::WindowFlags flags)
 : QMainWindow(parent, flags)
{
  setupUi(this);
  pvForm = 0;
  /**
  actSubmit = 0;
  actRevert = 0;
  actInsert = 0;
  actRemove = 0;
  */
  createEditionElements();
  tlbMain->addSeparator();
  createNavigationElements();
  /**
  actNavToFirst = 0;
  actNavToLast = 0;
  actNavToPrevious = 0;
  actNavToNext = 0;
  */
}

mdtSqlWindow::~mdtSqlWindow()
{
}

/**
void mdtSqlWindow::setTableController(std::shared_ptr< mdtSqlDataWidgetController > controller)
{
  Q_ASSERT(controller);

  pvController = controller;
  connectEditionElements();
  connectNavigationElements();
  pvController->start();
}
*/

void mdtSqlWindow::setSqlForm(mdtSqlForm *form)
{
  Q_ASSERT(form != 0);

  pvForm = form;
  setCentralWidget(pvForm);
  connectEditionElements();
  connectNavigationElements();
  pvForm->start();
}

void mdtSqlWindow::enableNavigation()
{
  actNavToFirst->setVisible(true);
  actNavToPrevious->setVisible(true);
  actNavToNext->setVisible(true);
  actNavToLast->setVisible(true);
}

void mdtSqlWindow::disableNavigation()
{
  actNavToFirst->setVisible(false);
  actNavToPrevious->setVisible(false);
  actNavToNext->setVisible(false);
  actNavToLast->setVisible(false);
}

void mdtSqlWindow::enableEdition()
{
  actInsert->setVisible(true);
  actSubmit->setVisible(true);
  actRevert->setVisible(true);
  actRemove->setVisible(true);
}

void mdtSqlWindow::disableEdition()
{
  actInsert->setVisible(false);
  actSubmit->setVisible(false);
  actRevert->setVisible(false);
  actRemove->setVisible(false);
}

void mdtSqlWindow::createEditionElements()
{
  actInsert = new QAction(tr("New"), this);
  actInsert->setIcon(QIcon::fromTheme("document-new"));
  actSubmit = new QAction(tr("Save"), this);
  actSubmit->setIcon(QIcon::fromTheme("document-save"));
  actRevert = new QAction(tr("Cancel"), this);
  actRevert->setIcon(QIcon::fromTheme("document-revert"));
  actRemove = new QAction(tr("Delete"), this);
  actRemove->setIcon(QIcon::fromTheme("edit-delete"));
  // Setup shortcuts
  actInsert->setShortcut(QKeySequence::New);
  actSubmit->setShortcut(QKeySequence::Save);
  ///actRevert->setShortcut(QKeySequence::Undo);
  ///actRemove->setShortcut(QKeySequence::Delete);
  // As default, functions are disabled
  actInsert->setEnabled(false);
  actSubmit->setEnabled(false);
  actRevert->setEnabled(false);
  actRemove->setEnabled(false);
  // Add actions to toolbar
  tlbMain->addAction(actInsert);
  tlbMain->addAction(actSubmit);
  tlbMain->addAction(actRevert);
  tlbMain->addAction(actRemove);
}

void mdtSqlWindow::connectEditionElements()
{
  Q_ASSERT(pvForm != 0);

  /**
  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);
  */

  std::shared_ptr<mdtSqlDataWidgetController> controller;

  controller = pvForm->mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(controller);

  // Connect actions enable/disable
  connect(controller.get(), SIGNAL(insertEnabledStateChanged(bool)), actInsert, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(submitEnabledStateChanged(bool)), actSubmit, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  /**
  connect(widget, SIGNAL(insertEnabledStateChanged(bool)), actInsert, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(submitEnabledStateChanged(bool)), actSubmit, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  */
  // Connect actions triggers
  connect(actInsert, SIGNAL(triggered()), controller.get(), SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), controller.get(), SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), controller.get(), SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), controller.get(), SLOT(remove()));
  /**
  connect(actInsert, SIGNAL(triggered()), widget, SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), widget, SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), widget, SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), widget, SLOT(remove()));
  */
}

void mdtSqlWindow::createNavigationElements()
{
  actNavToFirst = new QAction("|<<", this);
  actNavToFirst->setIcon(QIcon::fromTheme("go-first"));
  actNavToPrevious = new QAction("<", this);
  actNavToPrevious->setIcon(QIcon::fromTheme("go-previous"));
  actNavToPrevious->setShortcut(Qt::ALT + Qt::Key_Left);
  actNavToNext = new QAction(">", this);
  actNavToNext->setIcon(QIcon::fromTheme("go-next"));
  actNavToNext->setShortcut(Qt::ALT + Qt::Key_Right);
  actNavToLast = new QAction(">>|", this);
  actNavToLast->setIcon(QIcon::fromTheme("go-last"));
  // Add actions to toolbar
  tlbMain->addAction(actNavToFirst);
  tlbMain->addAction(actNavToPrevious);
  tlbMain->addAction(actNavToNext);
  tlbMain->addAction(actNavToLast);
}

void mdtSqlWindow::connectNavigationElements()
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

  // Connect actions enable/disable
  connect(controller.get(), SIGNAL(toFirstEnabledStateChanged(bool)), actNavToFirst, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toPreviousEnabledStateChanged(bool)), actNavToPrevious, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toNextEnabledStateChanged(bool)), actNavToNext, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toLastEnabledStateChanged(bool)), actNavToLast, SLOT(setEnabled(bool)));
  /**
  connect(widget, SIGNAL(toFirstEnabledStateChanged(bool)), actNavToFirst, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toPreviousEnabledStateChanged(bool)), actNavToPrevious, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toNextEnabledStateChanged(bool)), actNavToNext, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toLastEnabledStateChanged(bool)), actNavToLast, SLOT(setEnabled(bool)));
  */
  // Connect actions triggers
  connect(actNavToFirst, SIGNAL(triggered()), controller.get(), SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), controller.get(), SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), controller.get(), SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), controller.get(), SLOT(toLast()));
  /**
  connect(actNavToFirst, SIGNAL(triggered()), widget, SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), widget, SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), widget, SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), widget, SLOT(toLast()));
  */
}

void mdtSqlWindow::closeEvent(QCloseEvent *event)
{
  Q_ASSERT(event != 0);

  // Check that all data are saved
  if((pvForm != 0) && (!pvForm->allDataAreSaved())){
    event->ignore();
  }else{
    event->accept();
  }
}
