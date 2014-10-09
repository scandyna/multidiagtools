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
#include "mdtTtBasicTestNodeCalibrationWindow.h"
#include "mdtTtBasicTestNodeCalibrationTool.h"
#include "mdtSqlTableWidget.h"
#include <QDate>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <memory>

#include <QDebug>

mdtTtBasicTestNodeCalibrationWindow::mdtTtBasicTestNodeCalibrationWindow(QSqlDatabase db, QWidget* parent)
 : QMainWindow(parent)
{
  QVBoxLayout *l;

  setupUi(this);
  // Setup tool
  pvCalibrationTool = new mdtTtBasicTestNodeCalibrationTool(db, this);
  // Add test node unit view
  l = new QVBoxLayout;
  pvTestNodeUnitWidget = new mdtSqlTableWidget;
  l->addWidget(pvTestNodeUnitWidget);
  wTestNodeUnit->setLayout(l);
}

bool mdtTtBasicTestNodeCalibrationWindow::init()
{
  connectActions();
  if(!pvCalibrationTool->setup(centralWidget())){
    return false;
  }
  pvTestNodeUnitWidget->setTableController(pvCalibrationTool->testNodeUnitViewTableController());
  // Go to test node
  /// \todo Make a Key field or something similar
  if(!pvCalibrationTool->setTestNode(2)){
    return false;
  }

  return true;
}

void mdtTtBasicTestNodeCalibrationWindow::connectActions()
{
  connect(actRunCalibration, SIGNAL(triggered()), pvCalibrationTool, SLOT(runCalibration()));
}
