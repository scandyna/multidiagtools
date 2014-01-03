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
#include "mdtTtCableChecker.h"
#include "ui_mdtTtCableChecker.h"
#include "mdtTtTest.h"
#include "mdtTtTestResult.h"
#include "mdtDevice.h"
#include "mdtDeviceModbusWago.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceInfo.h"
#include "mdtPortStatusWidget.h"
#include "mdtPortManager.h"
#include "mdtAbstractPort.h"
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
///#include <QString>
#include <QList>
///#include <QItemSelectionModel>
#include <QMessageBox>
///#include <QInputDialog>
#include <QWidget>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

using namespace mdtTtCableCheckerPrivate;

struct mdtTtCableCheckerPrivate::deviceStatusWidget
{
  QLabel *label;
  mdtPortStatusWidget *statusWidget;
};



mdtTtCableChecker::mdtTtCableChecker(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
  pvMultimeter = 0;
  pvDeviceStatusWidgetsLayout = 0;
}

bool mdtTtCableChecker::setupTables()
{
  if(!setupTestResultTable()){
    return false;
  }
  if(!setupTestResultItemTable()){
    return false;
  }
  createMultimeter();
  return true;
}

void mdtTtCableChecker::setBaseTest()
{
  mdtTtTestResult tr(database());
  QVariant testResultId;
  QVariant baseTestId;

  // Get current test result ID
  testResultId = currentData("TestResult_tbl", "Id_PK");
  if(testResultId.isNull()){
    return;
  }
  // Select a base test
  baseTestId = selectBaseTest();
  if(baseTestId.isNull()){
    return;
  }
  // Set base test
  if(!tr.setBaseTest(testResultId, baseTestId)){
    return;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  /**
  if(!setCurrentData("TestResult_tbl", "Test_Id_FK", baseTestId)){
    return;
  }
  */
}

void mdtTtCableChecker::removeTestResult()
{
  mdtTtTestResult t(database());
  QMessageBox msgBox;
  QVariant testResultId;

  // Get current test result ID
  testResultId = currentData("TestResult_tbl", "Id_PK");
  if(testResultId.isNull()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test result."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete test result items
  if(!t.removeData("TestResultItem_tbl", "TestResult_Id_FK", testResultId)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Delete test result
  mainSqlWidget()->remove();
  // Update connections table
  ///form()->select("TestItem_view");
}

void mdtTtCableChecker::runTest()
{
  if(!connectToInstruments()){
    displayLastError();
    return;
  }
  // Setup multimeter as Ohmmeter
  
  /// \todo Tester absence de tension
}

QVariant mdtTtCableChecker::selectBaseTest()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM Test_tbl";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of tests."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test"));
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

bool mdtTtCableChecker::setupTestResultTable() 
{
  Ui::mdtTtCableChecker cc;
  QSqlTableModel *testResultModel;
  QDataWidgetMapper *baseTestMapper;
  QSqlTableModel *baseTestModel;
  mdtSqlRelation *baseTestRelation;

  // Setup main form widget
  cc.setupUi(mainSqlWidget());
  // Setup form
  if(!setMainTable("TestResult_tbl", "Test result"/*, database()*/)){
    return false;
  }
  /*
   * Setup base test widget mapping
   */
  testResultModel = model("TestResult_tbl");
  Q_ASSERT(testResultModel != 0);
  // Setup base test model
  baseTestModel = new QSqlTableModel(this, database());
  baseTestModel->setTable("Test_tbl");
  if(!baseTestModel->select()){
    return false;
  }
  // Setup base test widget mapper
  baseTestMapper = new QDataWidgetMapper(this);
  baseTestMapper->setModel(baseTestModel);
  baseTestMapper->addMapping(cc.leTestDesignationEN, baseTestModel->fieldIndex("DesignationEN"));
  // Setup base test relation
  baseTestRelation = new mdtSqlRelation(this);
  baseTestRelation->setParentModel(testResultModel);
  baseTestRelation->setChildModel(baseTestModel);
  if(!baseTestRelation->addRelation("Test_Id_FK", "Id_PK", false)){
    return false;
  }
  connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseTestRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseTestRelation, SIGNAL(childModelFilterApplied()), baseTestMapper, SLOT(toFirst()));
  connect(baseTestRelation, SIGNAL(childModelIsEmpty()), baseTestMapper, SLOT(revert()));
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtCableChecker::setupTestResultItemTable() 
{
  mdtSqlTableWidget *widget;

  if(!addChildTable("TestResultItem_view", tr("Test result items") /*,database()*/)){
    return false;
  }
  if(!addRelation("Id_PK", "TestResultItem_view", "TestResult_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestResultItem_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Test_Id_FK", true);
  widget->setColumnHidden("TestResult_Id_FK", true);
  widget->setColumnHidden("TestItem_Id_FK", true);
  widget->setColumnHidden("TestConnectorNameBusA", true);
  widget->setColumnHidden("TestContactNameBusA", true);
  widget->setColumnHidden("TestConnectorNameBusB", true);
  widget->setColumnHidden("TestContactNameBusB", true);
  // Set fields a user friendly name
  widget->setHeaderData("SequenceNumber", tr("Seq. #"));
  widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  widget->setHeaderData("MeasuredValue", tr("Value\nMeasured"));
  widget->setHeaderData("TestConnectorNameBusA", tr("Test\nConnector\nBus A"));
  widget->setHeaderData("TestConnectorNameBusB", tr("Test\nConnector\nBus B"));
  widget->setHeaderData("TestContactNameBusA", tr("Test\nContact\nBus A"));
  widget->setHeaderData("TestContactNameBusB", tr("Test\nContact\nBus B"));
  widget->setHeaderData("DutUnitSchemaPositionBusA", tr("DUT BUS A"));
  widget->setHeaderData("DutUnitSchemaPositionBusB", tr("DUT BUS B"));
  widget->setHeaderData("DutConnectorNameBusA", tr("DUT\nConnector\nBus A"));
  widget->setHeaderData("DutConnectorNameBusB", tr("DUT\nConnector\nBus B"));
  widget->setHeaderData("DutContactNameBusA", tr("DUT\nContact\nBus A"));
  widget->setHeaderData("DutContactNameBusB", tr("DUT\nContact\nBus B"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  /**
  pbSetTestLink = new QPushButton(tr("Set test link ..."));
  connect(pbSetTestLink, SIGNAL(clicked()), this, SLOT(setTestLink()));
  widget->addWidgetToLocalBar(pbSetTestLink);
  widget->addStretchToLocalBar();
  */

  return true;
}

void mdtTtCableChecker::createMultimeter()
{
  pvMultimeter = new mdtDeviceU3606A;
  pvMultimeter->setName("U3606A");
  addDeviceStatusWidget(pvMultimeter, "U3606A");
}

bool mdtTtCableChecker::connectToInstruments()
{
  Q_ASSERT(pvMultimeter != 0);

  if(pvMultimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to U3606A"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    return false;
  }

  return true;
}

void mdtTtCableChecker::addDeviceStatusWidget(mdtDevice *device, const QString & label)
{
  Q_ASSERT(device != 0);

  deviceStatusWidget *dsw;
  int row;

  // Create status widget page, if needed
  if(pvDeviceStatusWidgetsLayout == 0){
    QWidget *w = new QWidget;
    QVBoxLayout *l = new QVBoxLayout;
    pvDeviceStatusWidgetsLayout = new QGridLayout;
    l->addLayout(pvDeviceStatusWidgetsLayout);
    l->addStretch();
    w->setLayout(l);
    addChildWidget(w, tr("Instruments states"));
  }
  Q_ASSERT(pvDeviceStatusWidgetsLayout != 0);
  // Create status widget
  dsw = new deviceStatusWidget;
  dsw->label = new QLabel(label);
  dsw->statusWidget = new mdtPortStatusWidget;
  pvDeviceStatusWidgets.insert(device, dsw);
  // Layout status widget and label
  row = pvDeviceStatusWidgetsLayout->rowCount();
  pvDeviceStatusWidgetsLayout->addWidget(dsw->label, row, 0, Qt::AlignVCenter);
  pvDeviceStatusWidgetsLayout->addWidget(dsw->statusWidget, row, 1, Qt::AlignVCenter);
  // make connections
  connect(device, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), dsw->statusWidget, SLOT(showMessage(const QString&, const QString&, int)));
  Q_ASSERT(device->portManager() != 0);
  connect(device->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), dsw->statusWidget, SLOT(setState(int, const QString&, int, bool)));
  device->portManager()->notifyCurrentState();
}

void mdtTtCableChecker::removeDeviceStatusWidget(mdtDevice *device)
{
  deviceStatusWidget *dsw;

  dsw = pvDeviceStatusWidgets.value(device, 0);
  if(dsw != 0){
    delete dsw->label;
    delete dsw->statusWidget;
    delete dsw;
    pvDeviceStatusWidgets.remove(device);
  }
}
