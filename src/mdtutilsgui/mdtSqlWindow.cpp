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
}

mdtSqlWindow::~mdtSqlWindow()
{
}

void mdtSqlWindow::setSqlWidget(mdtAbstractSqlWidget *sqlWidget)
{
  Q_ASSERT(sqlWidget != 0);

  setCentralWidget(sqlWidget);
}

void mdtSqlWindow::enableNavigation()
{
  Q_ASSERT(centralWidget() != 0);

  // We need a mdtAbstractSqlWidget pointer
  mdtAbstractSqlWidget *sqlWidget = dynamic_cast<mdtAbstractSqlWidget*>(centralWidget());
  Q_ASSERT(sqlWidget != 0);

  // Create actions
  actNavToFirst = new QAction("|<<", this);
  actNavToPrevious = new QAction("<", this);
  actNavToNext = new QAction(">", this);
  actNavToLast = new QAction(">>|", this);
  connect(actNavToFirst, SIGNAL(triggered()), sqlWidget, SLOT(toFirst()));
  connect(actNavToPrevious, SIGNAL(triggered()), sqlWidget, SLOT(toPrevious()));
  connect(actNavToNext, SIGNAL(triggered()), sqlWidget, SLOT(toNext()));
  connect(actNavToLast, SIGNAL(triggered()), sqlWidget, SLOT(toLast()));
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
  Q_ASSERT(centralWidget() != 0);

  // We need a mdtAbstractSqlWidget pointer
  mdtAbstractSqlWidget *sqlWidget = dynamic_cast<mdtAbstractSqlWidget*>(centralWidget());
  Q_ASSERT(sqlWidget != 0);

  // Create actions
  actInsert = new QAction(tr("New"), this);
  actSubmit = new QAction(tr("Save"), this);
  actRevert = new QAction(tr("Cancel"), this);
  actRemove = new QAction(tr("Delete"), this);
  connect(actInsert, SIGNAL(triggered()), sqlWidget, SLOT(insert()));
  connect(actSubmit, SIGNAL(triggered()), sqlWidget, SLOT(submit()));
  connect(actRevert, SIGNAL(triggered()), sqlWidget, SLOT(revert()));
  connect(actRemove, SIGNAL(triggered()), sqlWidget, SLOT(remove()));
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

