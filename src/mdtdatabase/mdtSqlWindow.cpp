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
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"
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
  actSubmit = 0;
  actRevert = 0;
  actInsert = 0;
  actRemove = 0;
  actNavToFirst = 0;
  actNavToLast = 0;
  actNavToPrevious = 0;
  actNavToNext = 0;
  setupUi(this);
  pvForm = 0;
}

mdtSqlWindow::~mdtSqlWindow()
{
}

void mdtSqlWindow::setSqlForm(mdtSqlForm *form)
{
  Q_ASSERT(form != 0);

  pvForm = form;
  setCentralWidget(pvForm);
}

void mdtSqlWindow::enableNavigation()
{
  Q_ASSERT(pvForm != 0);

  // Check if navigation was allready enabled
  if(actNavToFirst != 0){
    return;
  }

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);

  // Create actions
  actNavToFirst = new QAction("|<<", this);
  actNavToFirst->setIcon(QIcon::fromTheme("go-first"));
  actNavToPrevious = new QAction("<", this);
  actNavToPrevious->setIcon(QIcon::fromTheme("go-previous"));
  actNavToNext = new QAction(">", this);
  actNavToNext->setIcon(QIcon::fromTheme("go-next"));
  actNavToLast = new QAction(">>|", this);
  actNavToLast->setIcon(QIcon::fromTheme("go-last"));
  // Connect actions enable/disable
  connect(widget, SIGNAL(toFirstEnabledStateChanged(bool)), actNavToFirst, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toPreviousEnabledStateChanged(bool)), actNavToPrevious, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toNextEnabledStateChanged(bool)), actNavToNext, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(toLastEnabledStateChanged(bool)), actNavToLast, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actNavToFirst, SIGNAL(triggered()), widget, SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), widget, SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), widget, SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), widget, SLOT(toLast()));
  // Add a separator if edition is allready on toolbar
  if(actRemove != 0){
    tlbMain->addSeparator();
  }
  // Add actions to toolbar
  tlbMain->addAction(actNavToFirst);
  tlbMain->addAction(actNavToPrevious);
  tlbMain->addAction(actNavToNext);
  tlbMain->addAction(actNavToLast);
}

void mdtSqlWindow::disableNavigation()
{
  // Check if navigation was allready diseabled
  if(actNavToFirst == 0){
    return;
  }
  tlbMain->removeAction(actNavToFirst);
  delete actNavToFirst;
  actNavToFirst = 0;
  tlbMain->removeAction(actNavToPrevious);
  delete actNavToPrevious;
  actNavToPrevious = 0;
  tlbMain->removeAction(actNavToNext);
  delete actNavToNext;
  actNavToNext = 0;
  tlbMain->removeAction(actNavToLast);
  delete actNavToLast;
  actNavToLast = 0;
  // Remove actions from toolbar
  /// \todo Possible ?
}

void mdtSqlWindow::enableEdition()
{
  Q_ASSERT(pvForm != 0);

  mdtSqlFormWidget *widget = pvForm->mainSqlWidget();
  Q_ASSERT(widget != 0);

  // Create actions
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
  // Connect actions enable/disable
  connect(widget, SIGNAL(insertEnabledStateChanged(bool)), actInsert, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(submitEnabledStateChanged(bool)), actSubmit, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  connect(widget, SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actInsert, SIGNAL(triggered()), widget, SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), widget, SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), widget, SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), widget, SLOT(remove()));
  // Add a separator if navigation is allready on toolbar
  if(actNavToLast != 0){
    tlbMain->addSeparator();
  }
  // Add actions to toolbar
  tlbMain->addAction(actInsert);
  tlbMain->addAction(actSubmit);
  tlbMain->addAction(actRevert);
  tlbMain->addAction(actRemove);
}

void mdtSqlWindow::disableEdition()
{
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
