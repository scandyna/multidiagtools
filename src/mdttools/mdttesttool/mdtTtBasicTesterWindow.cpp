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
#include "mdtTtBasicTesterWindow.h"
#include "mdtTtBasicTester.h"
#include "mdtDeviceContainerWidget.h"
#include "mdtSqlTableWidget.h"
#include <QDate>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <memory>

#include <QDebug>

mdtTtBasicTesterWindow::mdtTtBasicTesterWindow(QSqlDatabase db, QWidget* parent)
 : QMainWindow(parent)
{
  QVBoxLayout *l;

  setupUi(this);
  // Setup tester
  pvTester = new mdtTtBasicTester(db, this);
  ///connect(pvTester, SIGNAL(testDataChanged(const QSqlRecord&)), this, SLOT(displayTestData(const QSqlRecord&)));
  // Setup instruments state widget
  l = new QVBoxLayout;
  pvDeviceContainerWidget = new mdtDeviceContainerWidget;
  pvDeviceContainerWidget->setContainer(pvTester->testNodeManager()->container());
  l->addWidget(pvDeviceContainerWidget);
  l->addStretch();
  tbInstruments->setLayout(l);
  // Setup test item table widget
  pvTestItemWidget = new mdtSqlTableWidget;
  /// \todo Adapter !
  ///pvTestItemWidget->setModel(pvTester->testItemViewTableModel().get());
  l = new QVBoxLayout;
  l->addWidget(pvTestItemWidget);
  tbTestItem->setLayout(l);
  
  // Actions
  connectActions();
}

/// \todo Error dialog !!
bool mdtTtBasicTesterWindow::init()
{
  ///Q_ASSERT(pvTester->test()->testViewController());

  /**
  if(!pvTester->init()){
    return false;
  }
  */
  if(!pvTester->setup()){
    return false;
  }
  if(!pvTester->setTestUiWidget(centralWidget())){
    return false;
  }
  setupTestItemTableWidget();
  ///pvTester->test()->testViewController()->start();
  if(!pvTester->start()){
    return false;
  }

  return true;
}

mdtError mdtTtBasicTesterWindow::lastError() const
{
  return pvTester->lastError();
}

/**
void mdtTtBasicTesterWindow::displayTestData(const QSqlRecord & data)
{
  lbTestDesignationEN->setText(data.value("DesignationEN").toString());
}
*/

void mdtTtBasicTesterWindow::setupTestItemTableWidget()
{
  Q_ASSERT(pvTester->testItemViewController());

  pvTestItemWidget->setTableController(pvTester->testItemViewController());
  connect(pvTester->testViewController().get(), SIGNAL(childWidgetEnableStateChanged(bool)), pvTestItemWidget, SLOT(setEnabled(bool)));
  // Hide technical fields
  pvTestItemWidget->setColumnHidden("Id_PK", true);
  pvTestItemWidget->setColumnHidden("Test_Id_FK", true);
  pvTestItemWidget->setColumnHidden("TestModelItem_Id_FK", true);
  // Set fields some user friendly names
  pvTestItemWidget->setHeaderData("SequenceNumber", tr("Seq. #"));
  pvTestItemWidget->setHeaderData("DesignationEN", tr("Designation"));
  pvTestItemWidget->setHeaderData("MeasuredValue", tr("Measured\nvalue"));
  pvTestItemWidget->setHeaderData("InstrumentRangeMin", tr("Instrument\nrange\nmin"));
  pvTestItemWidget->setHeaderData("InstrumentRangeMax", tr("Instrument\nrange\nmax"));
  pvTestItemWidget->setHeaderData("ResultValue", tr("Result\nvalue"));
  pvTestItemWidget->setHeaderData("LimitValueMin", tr("Limit\nvalue\nmin"));
  pvTestItemWidget->setHeaderData("LimitValueMax", tr("Limit\nvalue\nmax"));
  pvTestItemWidget->setHeaderData("FailValueMax", tr("Fail\nvalue\nmax"));
  pvTestItemWidget->setHeaderData("FailValueMin", tr("Fail\nvalue\nmin"));
  pvTestItemWidget->setHeaderData("ExpectedValue", tr("Expected\nvalue"));
  // Other things
  pvTestItemWidget->addColumnToSortOrder("SequenceNumber", Qt::AscendingOrder);
  pvTestItemWidget->sort();
  pvTestItemWidget->resizeViewToContents();
}

void mdtTtBasicTesterWindow::connectActions()
{
  Q_ASSERT(pvTester != 0);
  ///connect(actTestSetType, SIGNAL(triggered()), pvTester, SLOT(setTestModel()));
  connect(actTestSave, SIGNAL(triggered()), pvTester, SLOT(saveTest()));
  connect(actTestNew, SIGNAL(triggered()), pvTester, SLOT(createTest()));
  connect(actTestView, SIGNAL(triggered()), pvTester, SLOT(openTest()));
  connect(actTestRun, SIGNAL(triggered()), pvTester, SLOT(runTest()));
}

void mdtTtBasicTesterWindow::closeEvent(QCloseEvent* event)
{
  if(pvTester == 0){
    event->accept();
    return;
  }
  if(!pvTester->testIsSaved()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Current test was not saved."));
    msgBox.setInformativeText(tr("If you continue, current test will be lost. Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() == QMessageBox::Yes){
      event->accept();
    }else{
      event->ignore();
    }
  }else{
    event->accept();
  }
}
