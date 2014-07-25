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
#include <memory>

#include <QDebug>

mdtTtBasicTesterWindow::mdtTtBasicTesterWindow(QWidget* parent)
 : QMainWindow(parent)
{
  QVBoxLayout *l;

  pvTesterWidget = 0;
  setupUi(this);

  // Setup instruments state widget
  l = new QVBoxLayout;
  pvDeviceContainerWidget = new mdtDeviceContainerWidget;
  l->addWidget(pvDeviceContainerWidget);
  l->addStretch();
  tbInstruments->setLayout(l);
  // Setup test item table widget
  pvTestItemWidget = new mdtSqlTableWidget;
  l = new QVBoxLayout;
  l->addWidget(pvTestItemWidget);
  tbTestItem->setLayout(l);

}

void mdtTtBasicTesterWindow::setTesterWidget(mdtTtBasicTester* tester)
{
  Q_ASSERT(tester != 0);
  
  /// \todo If pvTesterWidget was allready set, disconnect...

  pvTesterWidget = tester;
  pvTesterWidget->setTestUiWidget(centralWidget());
  pvDeviceContainerWidget->setContainer(pvTesterWidget->testNodeManager()->container());
  pvTestItemWidget->setModel(pvTesterWidget->testItemViewTableModel().get());
  ///connect(pvTesterWidget, SIGNAL(testItemTableSet()), this, SLOT(setupTestItemTableWidget()));
  connect(pvTesterWidget, SIGNAL(testDataChanged(const QSqlRecord&)), this, SLOT(displayTestData(const QSqlRecord&)));
  ///connect(leSN, SIGNAL(textChanged(const QString&)), pvTesterWidget, SLOT(setDutSerialNumber(const QString&)));
  connectActions();
}

void mdtTtBasicTesterWindow::setupTestItemTableWidget()
{
  qDebug() << "mdtTtBasicTesterWindow::setupTestItemTableWidget()  - table: " << pvTesterWidget->testItemViewTableModel()->tableName();
  
  // Hide technical fields
  pvTestItemWidget->setColumnHidden("Id_PK", true);
  pvTestItemWidget->setColumnHidden("Test_Id_FK", true);
  pvTestItemWidget->setColumnHidden("TestModelItem_Id_FK", true);
  // Set fields some user friendly names
  pvTestItemWidget->setHeaderData("SequenceNumber", tr("Seq. #"));
  pvTestItemWidget->setHeaderData("DesignationEN", tr("Designation (English)"));
  pvTestItemWidget->setHeaderData("ExpectedValue", tr("Value\nexpected"));
  pvTestItemWidget->setHeaderData("MeasuredValue", tr("Value\nmeasured"));
  // Other things
  pvTestItemWidget->addColumnToSortOrder("SequenceNumber", Qt::AscendingOrder);
  pvTestItemWidget->sort();
  pvTestItemWidget->resizeViewToContents();
}

void mdtTtBasicTesterWindow::displayTestData(const QSqlRecord & data)
{
  QDate date = data.value("Date").toDate();
  lbDate->setText(date.toString(Qt::SystemLocaleLongDate));
  lbTestDesignationEN->setText(data.value("DesignationEN").toString());
  ///leSN->setText(data.value("DutSerialNumber").toString());
}

void mdtTtBasicTesterWindow::connectActions()
{
  Q_ASSERT(pvTesterWidget != 0);
  connect(actTestSetType, SIGNAL(triggered()), pvTesterWidget, SLOT(setTestModel()));
  connect(actTestSave, SIGNAL(triggered()), pvTesterWidget, SLOT(saveTest()));
  connect(actTestNew, SIGNAL(triggered()), pvTesterWidget, SLOT(createTest()));
  connect(actTestView, SIGNAL(triggered()), pvTesterWidget, SLOT(openTest()));
  connect(actTestRun, SIGNAL(triggered()), pvTesterWidget, SLOT(runTest()));
}
