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
#include "mdtTtCableCheckerWindow.h"
#include "mdtTtCableChecker.h"
#include "mdtSqlFormWidget.h"

mdtTtCableCheckerWindow::mdtTtCableCheckerWindow(QWidget *parent)
 : QMainWindow(parent)
{
  pvCableChecker = 0;
  setupUi(this);
}

void mdtTtCableCheckerWindow::setCableChecker(mdtTtCableChecker *cc) 
{
  Q_ASSERT(cc != 0);

  mdtSqlFormWidget *fw;

  pvCableChecker = cc;
  setCentralWidget(pvCableChecker);

  // Setup actions
  ///fw = pvCableChecker->mainSqlWidget();
  fw = 0;
  Q_ASSERT(fw != 0);
  // As default, functions are disabled
  ///actNew->setEnabled(false);
  ///actSubmit->setEnabled(false);
  ///actRevert->setEnabled(false);
  actSave->setEnabled(false);
  // Connect actions enable/disable
  connect(fw, SIGNAL(insertEnabledStateChanged(bool)), actNew, SLOT(setEnabled(bool)));
  connect(fw, SIGNAL(submitEnabledStateChanged(bool)), actSave, SLOT(setEnabled(bool)));
  ///connect(fw, SIGNAL(revertEnabledStateChanged(bool)), actRevert, SLOT(setEnabled(bool)));
  ///connect(fw, SIGNAL(removeEnabledStateChanged(bool)), actRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(actNew, SIGNAL(triggered()), fw, SLOT(insert()));
  connect(actSave, SIGNAL(triggered()), fw, SLOT(submit()));
  ///connect(actRevert, SIGNAL(triggered()), fw, SLOT(revert()));
  ///connect(actRemove, SIGNAL(triggered()), fw, SLOT(remove()));
  connect(actSetTest, SIGNAL(triggered()), pvCableChecker, SLOT(setTestModel()));
  connect(actRemove, SIGNAL(triggered()), pvCableChecker, SLOT(removeTestResult()));
  connect(actRunTest, SIGNAL(triggered()), pvCableChecker, SLOT(runTest()));

}

