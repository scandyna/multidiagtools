/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
  createEditionElements();
  tlbMain->addSeparator();
  createNavigationElements();
}

mdtSqlWindow::~mdtSqlWindow()
{
  qDebug() << "mdtSqlWindow::~mdtSqlWindow() - form: " << pvForm->objectName();
}

void mdtSqlWindow::setSqlForm(mdtSqlForm *form)
{
  Q_ASSERT(form != 0);

  pvForm = form;
  setCentralWidget(pvForm);
  connectEditionElements();
  connectNavigationElements();
  pvForm->addMenus(menuBar());
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

void mdtSqlWindow::enableInsertion()
{
  actInsert->setVisible(true);
}

void mdtSqlWindow::disableInsertion()
{
  actInsert->setVisible(false);
}

void mdtSqlWindow::enableDeletion()
{
  actRemove->setVisible(true);
}

void mdtSqlWindow::disableDeletion()
{
  actRemove->setVisible(false);
}

void mdtSqlWindow::enableEdition()
{
  actSubmit->setVisible(true);
  actRevert->setVisible(true);
}

void mdtSqlWindow::disableEdition()
{
  actSubmit->setVisible(false);
  actRevert->setVisible(false);
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

  std::shared_ptr<mdtSqlDataWidgetController> controller;

  controller = pvForm->mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(controller);
  // Connect actions enable/disable
  connect(controller.get(), SIGNAL(insertEnabledStateChanged(bool)), actInsert, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(submitEnabledStateChanged(bool)), actSubmit, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actInsert, SIGNAL(triggered()), controller.get(), SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), controller.get(), SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), controller.get(), SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), controller.get(), SLOT(remove()));
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

  std::shared_ptr<mdtSqlDataWidgetController> controller;

  controller = pvForm->mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(controller);
  // Connect actions enable/disable
  connect(controller.get(), SIGNAL(toFirstEnabledStateChanged(bool)), actNavToFirst, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toPreviousEnabledStateChanged(bool)), actNavToPrevious, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toNextEnabledStateChanged(bool)), actNavToNext, SLOT(setEnabled(bool)));
  connect(controller.get(), SIGNAL(toLastEnabledStateChanged(bool)), actNavToLast, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actNavToFirst, SIGNAL(triggered()), controller.get(), SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), controller.get(), SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), controller.get(), SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), controller.get(), SLOT(toLast()));
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
