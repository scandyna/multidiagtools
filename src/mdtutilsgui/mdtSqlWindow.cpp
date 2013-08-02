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
#include "mdtSqlWindow.h"
#include "mdtAbstractSqlWidget.h"
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QKeySequence>
#include <QVBoxLayout>

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
  pvChildsTabWidget = 0;
  pvMainSqlWidget = 0;
  // Setup central widget, on witch we cann put perent widget and childs widgets tab widget.
  QWidget *widget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  widget->setLayout(layout);
  setCentralWidget(widget);
}

mdtSqlWindow::~mdtSqlWindow()
{
}

void mdtSqlWindow::setSqlWidget(mdtAbstractSqlWidget *sqlWidget)
{
  Q_ASSERT(sqlWidget != 0);

  pvMainSqlWidget = sqlWidget;
  centralWidget()->layout()->addWidget(pvMainSqlWidget);
}

void mdtSqlWindow::addChildWidget(QWidget *widget, const QString &label)
{
  Q_ASSERT(widget != 0);

  // Setup tab widget if needed
  if(pvChildsTabWidget == 0){
    pvChildsTabWidget = new QTabWidget;
    centralWidget()->layout()->addWidget(pvChildsTabWidget);
  }
  Q_ASSERT(pvChildsTabWidget != 0);
  pvChildsTabWidget->addTab(widget, label);
}

void mdtSqlWindow::enableNavigation()
{
  Q_ASSERT(pvMainSqlWidget != 0);

  // Create actions
  actNavToFirst = new QAction("|<<", this);
  actNavToPrevious = new QAction("<", this);
  actNavToNext = new QAction(">", this);
  actNavToLast = new QAction(">>|", this);
  // Connect actions enable/disable
  connect(pvMainSqlWidget, SIGNAL(toFirstEnabledStateChanged(bool)), actNavToFirst, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(toPreviousEnabledStateChanged(bool)), actNavToPrevious, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(toNextEnabledStateChanged(bool)), actNavToNext, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(toLastEnabledStateChanged(bool)), actNavToLast, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actNavToFirst, SIGNAL(triggered()), pvMainSqlWidget, SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), pvMainSqlWidget, SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), pvMainSqlWidget, SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), pvMainSqlWidget, SLOT(toLast()));
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
  // Remove actions from toolbar
  /// \todo Possible ?
}

void mdtSqlWindow::enableEdition()
{
  Q_ASSERT(pvMainSqlWidget != 0);

  // Create actions
  actInsert = new QAction(tr("New"), this);
  actSubmit = new QAction(tr("Save"), this);
  actRevert = new QAction(tr("Cancel"), this);
  actRemove = new QAction(tr("Delete"), this);
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
  connect(pvMainSqlWidget, SIGNAL(insertEnabledStateChanged(bool)), actInsert, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(submitEnabledStateChanged(bool)), actSubmit, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  connect(pvMainSqlWidget, SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actInsert, SIGNAL(triggered()), pvMainSqlWidget, SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), pvMainSqlWidget, SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), pvMainSqlWidget, SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), pvMainSqlWidget, SLOT(remove()));
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
  if((pvMainSqlWidget != 0) && (!pvMainSqlWidget->allDataAreSaved())){
    event->ignore();
  }else{
    event->accept();
  }
}
